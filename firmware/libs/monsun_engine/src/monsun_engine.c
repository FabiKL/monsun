#include "monsun_engine.h"

#include <string.h>

void monsun_pump_init(monsun_pump_t *p, const monsun_pump_cfg_t *cfg)
{
    memset(p, 0, sizeof(*p));
    p->cfg = *cfg;
    p->mode = MONSUN_MODE_OFF;
}

void monsun_pump_set_mode(monsun_pump_t *p, monsun_mode_t mode)
{
    p->mode = mode;
}

void monsun_pump_reset_fault(monsun_pump_t *p)
{
    p->fault = MONSUN_FAULT_NONE;
}

static const monsun_input_t *get_input(const monsun_input_t *in, size_t n, int8_t idx)
{
    if (idx < 0 || (size_t)idx >= n) {
        return NULL;
    }
    return &in[idx];
}

static bool cmp_ok(monsun_cmp_t cmp, float value, float threshold)
{
    return cmp == MONSUN_ABOVE ? value > threshold : value < threshold;
}

/* Returns true if every configured interlock is satisfied. Sets *latch if a latching one is violated. */
static bool interlocks_ok(const monsun_pump_cfg_t *cfg, const monsun_input_t *in, size_t n, bool *latch)
{
    bool ok = true;
    for (size_t i = 0; i < MONSUN_MAX_INTERLOCKS; i++) {
        const monsun_interlock_t *il = &cfg->interlocks[i];
        if (il->input_idx < 0) {
            continue;
        }
        const monsun_input_t *src = get_input(in, n, il->input_idx);
        bool good = src != NULL && src->valid && cmp_ok(il->cmp, src->value, il->threshold);
        if (!good) {
            ok = false;
            if (il->latch_fault) {
                *latch = true;
            }
        }
    }
    return ok;
}

static bool slot_active(const monsun_slot_t *s, const monsun_clock_t *clk)
{
    if (!s->enabled || s->duration_s == 0) {
        return false;
    }
    uint32_t now_s = (uint32_t)clk->minute_of_day * 60u + clk->second;
    uint32_t start_s = (uint32_t)s->start_minute * 60u;
    uint32_t delta = (now_s + 86400u - start_s) % 86400u;
    if (delta >= s->duration_s) {
        return false;
    }
    /* A window started yesterday belongs to yesterday's weekday */
    uint8_t day = clk->weekday;
    if (now_s < start_s) {
        day = (uint8_t)((day + 6u) % 7u);
    }
    return (s->days_mask >> day) & 1u;
}

static bool timer_demand(const monsun_pump_cfg_t *cfg, const monsun_clock_t *clk)
{
    if (!clk->time_valid && !(clk->time_estimated && cfg->time_policy == MONSUN_TIME_ESTIMATE)) {
        return false;
    }
    for (size_t i = 0; i < MONSUN_MAX_SLOTS; i++) {
        if (slot_active(&cfg->slots[i], clk)) {
            return true;
        }
    }
    return false;
}

static bool level_demand(monsun_pump_t *p, const monsun_input_t *in, size_t n)
{
    const monsun_level_ctrl_t *lc = &p->cfg.level;
    const monsun_input_t *src = get_input(in, n, lc->input_idx);
    if (lc->input_idx < 0) {
        return false;
    }
    if (src == NULL || !src->valid) {
        p->level_demand = false;
        return false;
    }
    if (src->value < lc->start_below) {
        p->level_demand = true;
    } else if (src->value > lc->stop_above) {
        p->level_demand = false;
    }
    return p->level_demand;
}

static void stop(monsun_pump_t *p, uint32_t now_s)
{
    if (p->running) {
        p->running = false;
        p->off_since_s = now_s;
        p->ever_stopped = true;
    }
}

bool monsun_pump_step(monsun_pump_t *p, const monsun_input_t *inputs, size_t n_inputs,
                      const monsun_clock_t *clk)
{
    bool latch = false;
    bool permit = interlocks_ok(&p->cfg, inputs, n_inputs, &latch);
    bool lvl = level_demand(p, inputs, n_inputs);

    if (latch && p->running && p->fault == MONSUN_FAULT_NONE) {
        p->fault = MONSUN_FAULT_INTERLOCK;
    }

    if (p->fault != MONSUN_FAULT_NONE || p->mode == MONSUN_MODE_OFF || !permit) {
        stop(p, clk->mono_s);
        return false;
    }

    bool demand = (p->mode == MONSUN_MODE_ON) || (p->mode == MONSUN_MODE_AUTO && (lvl || timer_demand(&p->cfg, clk)));

    if (p->running && p->cfg.max_run_s > 0 && clk->mono_s - p->run_started_s >= p->cfg.max_run_s) {
        p->fault = MONSUN_FAULT_MAX_RUNTIME;
        stop(p, clk->mono_s);
        return false;
    }

    if (!demand) {
        stop(p, clk->mono_s);
        return false;
    }

    if (!p->running) {
        if (p->ever_stopped && clk->mono_s - p->off_since_s < p->cfg.min_off_s) {
            return false;
        }
        p->running = true;
        p->run_started_s = clk->mono_s;
    }
    return true;
}
