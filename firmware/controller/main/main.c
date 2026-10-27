#include <stdbool.h>

#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "board.h"
#include "monsun_engine.h"
#include "monsun_registry.h"

static const char *TAG = "monsun";

/* Engine input indices for the IBC tank prototype */
enum { IN_RESERVOIR_OK = 0, IN_IBC_FULL, IN_IBC_LEVEL, IN_IBC_LEVEL_MIN_OK, IN_COUNT };

static monsun_pump_t s_reservoir_pump;
static monsun_pump_t s_ibc_pump;

static void relay_init(int pin)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_OUTPUT,
    };
    /* Set the safe (off) level before enabling the output driver */
    gpio_set_level(pin, !MONSUN_RELAY_ACTIVE_LEVEL);
    gpio_config(&cfg);
    gpio_set_level(pin, !MONSUN_RELAY_ACTIVE_LEVEL);
}

static void level_init(int pin)
{
    gpio_config_t cfg = {
        .pin_bit_mask = 1ULL << pin,
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
    };
    gpio_config(&cfg);
}

static bool level_active(int pin)
{
    return gpio_get_level(pin) == MONSUN_LEVEL_ACTIVE_LEVEL;
}

static void relay_set(int pin, bool on)
{
    gpio_set_level(pin, on ? MONSUN_RELAY_ACTIVE_LEVEL : !MONSUN_RELAY_ACTIVE_LEVEL);
}

static void engine_setup(void)
{
    /* Reservoir pump fills the IBC: needs water in the reservoir (latching dry-run), stops when the IBC is full. */
    monsun_pump_cfg_t fill = {0};
    for (int i = 0; i < MONSUN_MAX_INTERLOCKS; i++) {
        fill.interlocks[i].input_idx = -1;
    }
    fill.max_run_s = 3600;
    fill.min_off_s = 60;
    fill.interlocks[0] = (monsun_interlock_t){IN_RESERVOIR_OK, MONSUN_ABOVE, 0.5f, true};
    fill.interlocks[1] = (monsun_interlock_t){IN_IBC_FULL, MONSUN_BELOW, 0.5f, false};
    fill.level = (monsun_level_ctrl_t){IN_IBC_LEVEL, 40.0f, 90.0f};
    monsun_pump_init(&s_reservoir_pump, &fill);

    /* IBC pump supplies the garden: only runs above the minimum level. Timer slots are configured from HA later. */
    monsun_pump_cfg_t supply = {0};
    for (int i = 0; i < MONSUN_MAX_INTERLOCKS; i++) {
        supply.interlocks[i].input_idx = -1;
    }
    supply.level.input_idx = -1;
    supply.max_run_s = 1800;
    supply.min_off_s = 60;
    supply.interlocks[0] = (monsun_interlock_t){IN_IBC_LEVEL_MIN_OK, MONSUN_ABOVE, 0.5f, true};
    monsun_pump_init(&s_ibc_pump, &supply);

    monsun_pump_set_mode(&s_reservoir_pump, MONSUN_MODE_AUTO);
    monsun_pump_set_mode(&s_ibc_pump, MONSUN_MODE_OFF);
}

static void registry_setup(void)
{
    const monsun_entity_t ents[] = {
        {"reservoir_pump", "Reservoir pump", MONSUN_ENT_BINARY_SENSOR, "running", NULL, false, false, 0, true},
        {"ibc_pump", "IBC pump", MONSUN_ENT_BINARY_SENSOR, "running", NULL, false, false, 0, true},
        {"reservoir_ok", "Reservoir not empty", MONSUN_ENT_BINARY_SENSOR, NULL, NULL, false, false, 0, true},
        {"ibc_full", "IBC full", MONSUN_ENT_BINARY_SENSOR, NULL, NULL, false, false, 0, true},
        {"ibc_level", "IBC level", MONSUN_ENT_SENSOR, NULL, "%", false, false, 0, false},
    };
    for (size_t i = 0; i < sizeof(ents) / sizeof(ents[0]); i++) {
        monsun_registry_add(&ents[i]);
    }
}

static void control_task(void *arg)
{
    (void)arg;
    TickType_t last = xTaskGetTickCount();

    for (;;) {
        monsun_input_t in[IN_COUNT] = {0};
        in[IN_RESERVOIR_OK] = (monsun_input_t){true, level_active(MONSUN_PIN_LEVEL_1) ? 1.0f : 0.0f};
        in[IN_IBC_FULL] = (monsun_input_t){true, level_active(MONSUN_PIN_LEVEL_2) ? 1.0f : 0.0f};
        /* The analog IBC level (ADS1115) is not implemented yet. It stays invalid, so the engine fails safe (pumps off). */
        in[IN_IBC_LEVEL] = (monsun_input_t){false, 0.0f};
        in[IN_IBC_LEVEL_MIN_OK] = (monsun_input_t){false, 0.0f};

        monsun_clock_t clk = {.mono_s = (uint32_t)(esp_timer_get_time() / 1000000LL)};

        bool a = monsun_pump_step(&s_reservoir_pump, in, IN_COUNT, &clk);
        bool b = monsun_pump_step(&s_ibc_pump, in, IN_COUNT, &clk);
        relay_set(MONSUN_PIN_RELAY_1, a);
        relay_set(MONSUN_PIN_RELAY_2, b);

        monsun_registry_set("reservoir_pump", a, true);
        monsun_registry_set("ibc_pump", b, true);
        monsun_registry_set("reservoir_ok", in[IN_RESERVOIR_OK].value, true);
        monsun_registry_set("ibc_full", in[IN_IBC_FULL].value, true);

        vTaskDelayUntil(&last, pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    ESP_LOGI(TAG, "Monsun controller starting");
    relay_init(MONSUN_PIN_RELAY_1);
    relay_init(MONSUN_PIN_RELAY_2);
    level_init(MONSUN_PIN_LEVEL_1);
    level_init(MONSUN_PIN_LEVEL_2);

    registry_setup();
    engine_setup();

    xTaskCreate(control_task, "control", 4096, NULL, 5, NULL);
}
