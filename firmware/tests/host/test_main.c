#include <stdio.h>
#include <string.h>

#include "monsun_engine.h"
#include "monsun_link.h"

static int failures;
#define CHECK(cond)                                                             \
    do {                                                                        \
        if (!(cond)) {                                                          \
            printf("FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond);              \
            failures++;                                                         \
        }                                                                       \
    } while (0)

static monsun_clock_t clk(uint32_t mono, uint8_t wd, uint16_t min, uint8_t sec, bool valid)
{
    monsun_clock_t c = {.mono_s = mono, .time_valid = valid, .weekday = wd, .minute_of_day = min, .second = sec};
    return c;
}

static monsun_pump_cfg_t empty_cfg(void)
{
    monsun_pump_cfg_t c;
    memset(&c, 0, sizeof(c));
    for (int i = 0; i < MONSUN_MAX_INTERLOCKS; i++) {
        c.interlocks[i].input_idx = -1;
    }
    c.level.input_idx = -1;
    return c;
}

static void test_crc_and_frames(void)
{
    /* Known Modbus example: 01 03 00 00 00 0A -> CRC C5 CD (low byte first on wire) */
    const uint8_t req[] = {0x01, 0x03, 0x00, 0x00, 0x00, 0x0A};
    CHECK(monsun_crc16(req, sizeof(req)) == 0xCDC5);

    uint8_t frame[16];
    size_t len = monsun_build_read_request(frame, sizeof(frame), 1, MB_FC_READ_HOLDING, 0, 10);
    CHECK(len == 8);
    CHECK(frame[6] == 0xC5 && frame[7] == 0xCD);

    uint8_t slave = 0;
    size_t pdu_len = 0;
    const uint8_t *pdu = monsun_parse_frame(frame, len, &slave, &pdu_len);
    CHECK(pdu != NULL && slave == 1 && pdu_len == 5);
    frame[3] ^= 1;
    CHECK(monsun_parse_frame(frame, len, &slave, &pdu_len) == NULL);
}

static void test_response_and_descriptor(void)
{
    monsun_entity_desc_t d = {MONSUN_ENT_SENSOR, 3, 2, 0x0100, MONSUN_REG_INPUT, -1, 0, 0xFFFF};
    uint16_t regs[MONSUN_LINK_DESC_REGS];
    monsun_desc_to_regs(&d, regs);

    uint8_t frame[64];
    frame[0] = 5;
    frame[1] = MB_FC_READ_HOLDING;
    frame[2] = MONSUN_LINK_DESC_REGS * 2;
    for (int i = 0; i < MONSUN_LINK_DESC_REGS; i++) {
        frame[3 + 2 * i] = (uint8_t)(regs[i] >> 8);
        frame[4 + 2 * i] = (uint8_t)(regs[i] & 0xFF);
    }
    size_t body = 3 + MONSUN_LINK_DESC_REGS * 2;
    uint16_t crc = monsun_crc16(frame, body);
    frame[body] = (uint8_t)(crc & 0xFF);
    frame[body + 1] = (uint8_t)(crc >> 8);

    uint8_t slave;
    size_t pdu_len;
    const uint8_t *pdu = monsun_parse_frame(frame, body + 2, &slave, &pdu_len);
    CHECK(pdu != NULL);
    uint16_t out[MONSUN_LINK_DESC_REGS];
    CHECK(monsun_parse_read_regs_response(pdu, pdu_len, out, MONSUN_LINK_DESC_REGS) == MONSUN_LINK_DESC_REGS);
    monsun_entity_desc_t d2;
    monsun_desc_from_regs(out, &d2);
    CHECK(d2.reg_addr == 0x0100 && d2.scale == -1 && d2.name_id == 0xFFFF);

    uint16_t hdr[6] = {MONSUN_LINK_MAGIC, 0x0000, 1, 0, 1, 4};
    CHECK(monsun_header_valid(hdr));
    hdr[0] = 0x1234;
    CHECK(!monsun_header_valid(hdr));
}

/* Reservoir pump fills the IBC: inputs 0 = reservoir not empty, 1 = IBC full switch (1 = full), 2 = IBC level % */
static void test_level_fill_with_interlocks(void)
{
    monsun_pump_cfg_t c = empty_cfg();
    c.interlocks[0] = (monsun_interlock_t){0, MONSUN_ABOVE, 0.5f, true};
    c.interlocks[1] = (monsun_interlock_t){1, MONSUN_BELOW, 0.5f, false};
    c.level = (monsun_level_ctrl_t){2, 40.0f, 90.0f};
    monsun_pump_t p;
    monsun_pump_init(&p, &c);
    monsun_pump_set_mode(&p, MONSUN_MODE_AUTO);

    monsun_input_t in[3] = {{true, 1}, {true, 0}, {true, 60}};
    monsun_clock_t t = clk(100, 0, 0, 0, false);
    CHECK(!monsun_pump_step(&p, in, 3, &t));      /* 60 % is between thresholds, no demand yet */
    in[2].value = 30;
    CHECK(monsun_pump_step(&p, in, 3, &t));       /* below start */
    in[2].value = 60;
    CHECK(monsun_pump_step(&p, in, 3, &t));       /* hysteresis keeps running */
    in[2].value = 95;
    CHECK(!monsun_pump_step(&p, in, 3, &t));      /* above stop */

    in[2].value = 30;
    in[1].value = 1;                              /* IBC full switch: no latch, just blocked */
    CHECK(!monsun_pump_step(&p, in, 3, &t));
    CHECK(p.fault == MONSUN_FAULT_NONE);
    in[1].value = 0;
    CHECK(monsun_pump_step(&p, in, 3, &t));

    in[0].value = 0;                              /* reservoir empty while running => dry-run fault */
    CHECK(!monsun_pump_step(&p, in, 3, &t));
    CHECK(p.fault == MONSUN_FAULT_INTERLOCK);
    in[0].value = 1;
    CHECK(!monsun_pump_step(&p, in, 3, &t));      /* latched until reset */
    monsun_pump_reset_fault(&p);
    CHECK(monsun_pump_step(&p, in, 3, &t));

    in[0].valid = false;                          /* stale safety input => off */
    CHECK(!monsun_pump_step(&p, in, 3, &t));
}

static void test_timer_and_runtime(void)
{
    monsun_pump_cfg_t c = empty_cfg();
    c.max_run_s = 600;
    c.min_off_s = 60;
    c.slots[0] = (monsun_slot_t){true, 0x1F /* Mon-Fri */, 6 * 60, 300};
    monsun_pump_t p;
    monsun_pump_init(&p, &c);
    monsun_pump_set_mode(&p, MONSUN_MODE_AUTO);

    monsun_clock_t t = clk(1000, 1, 6 * 60, 10, true);  /* Tuesday 06:00:10 */
    CHECK(monsun_pump_step(&p, NULL, 0, &t));
    t = clk(1400, 1, 6 * 60 + 5, 1, true);              /* window over */
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));
    t = clk(1401, 5, 6 * 60, 10, true);                 /* Saturday: not in mask */
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));

    t = clk(2000, 1, 6 * 60, 10, false);                /* clock not valid, policy wait */
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));
    t.time_estimated = true;
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));
    p.cfg.time_policy = MONSUN_TIME_ESTIMATE;
    CHECK(monsun_pump_step(&p, NULL, 0, &t));

    /* manual on is limited by max runtime and then faults */
    monsun_pump_init(&p, &c);
    monsun_pump_set_mode(&p, MONSUN_MODE_ON);
    t = clk(0, 0, 0, 0, true);
    CHECK(monsun_pump_step(&p, NULL, 0, &t));
    t.mono_s = 601;
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));
    CHECK(p.fault == MONSUN_FAULT_MAX_RUNTIME);
}

static void test_window_across_midnight_and_min_off(void)
{
    monsun_pump_cfg_t c = empty_cfg();
    c.min_off_s = 100;
    c.slots[0] = (monsun_slot_t){true, 0x01 /* Monday only */, 23 * 60 + 50, 1200};
    monsun_pump_t p;
    monsun_pump_init(&p, &c);
    monsun_pump_set_mode(&p, MONSUN_MODE_AUTO);

    monsun_clock_t t = clk(10, 0, 23 * 60 + 55, 0, true);   /* Monday 23:55 */
    CHECK(monsun_pump_step(&p, NULL, 0, &t));
    t = clk(20, 1, 0, 3, true);                              /* Tuesday 00:03, window began Monday */
    CHECK(monsun_pump_step(&p, NULL, 0, &t));
    t = clk(30, 1, 23 * 60 + 55, 0, true);                   /* Tuesday 23:55, not in mask */
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));

    monsun_pump_set_mode(&p, MONSUN_MODE_ON);                /* restart immediately blocked by min off */
    t = clk(40, 1, 12 * 60, 0, true);
    CHECK(!monsun_pump_step(&p, NULL, 0, &t));
    t.mono_s = 140;
    CHECK(monsun_pump_step(&p, NULL, 0, &t));
}

int main(void)
{
    test_crc_and_frames();
    test_response_and_descriptor();
    test_level_fill_with_interlocks();
    test_timer_and_runtime();
    test_window_across_midnight_and_min_off();
    if (failures == 0) {
        printf("all tests passed\n");
    }
    return failures ? 1 : 0;
}
