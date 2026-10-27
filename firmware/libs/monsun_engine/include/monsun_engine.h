#ifndef MONSUN_ENGINE_H
#define MONSUN_ENGINE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MONSUN_MAX_INTERLOCKS 4
#define MONSUN_MAX_SLOTS      4

typedef enum { MONSUN_MODE_OFF = 0, MONSUN_MODE_AUTO = 1, MONSUN_MODE_ON = 2 } monsun_mode_t;

typedef enum {
    MONSUN_FAULT_NONE = 0,
    MONSUN_FAULT_INTERLOCK = 1,   /* a latching interlock was violated while running */
    MONSUN_FAULT_MAX_RUNTIME = 2,
} monsun_fault_t;

typedef enum { MONSUN_TIME_WAIT = 0, MONSUN_TIME_ESTIMATE = 1 } monsun_time_policy_t;

/* A sensor value with validity (false = stale, missing or failed). Binary inputs use 0.0 / 1.0. */
typedef struct {
    bool valid;
    float value;
} monsun_input_t;

typedef struct {
    uint32_t mono_s;          /* monotonic seconds since boot */
    bool time_valid;          /* wall clock synchronized (SNTP / HA) */
    bool time_estimated;      /* wall clock restored from NVS, not yet synced */
    uint8_t weekday;          /* 0 = Monday ... 6 = Sunday */
    uint16_t minute_of_day;   /* 0..1439 */
    uint8_t second;           /* 0..59 */
} monsun_clock_t;

typedef enum { MONSUN_ABOVE = 0, MONSUN_BELOW = 1 } monsun_cmp_t;

/* The actuator may only run while input[input_idx] is valid and above/below threshold. */
typedef struct {
    int8_t input_idx;         /* -1 = unused */
    monsun_cmp_t cmp;
    float threshold;
    bool latch_fault;         /* violation while running latches MONSUN_FAULT_INTERLOCK (e.g. dry run) */
} monsun_interlock_t;

typedef struct {
    bool enabled;
    uint8_t days_mask;        /* bit0 = Monday ... bit6 = Sunday */
    uint16_t start_minute;    /* minute of day */
    uint32_t duration_s;
} monsun_slot_t;

/* Fill/drain control with hysteresis on one input. demand starts below start_below, ends above stop_above. */
typedef struct {
    int8_t input_idx;         /* -1 = no level control */
    float start_below;
    float stop_above;
} monsun_level_ctrl_t;

typedef struct {
    uint32_t max_run_s;       /* 0 = unlimited */
    uint32_t min_off_s;
    monsun_time_policy_t time_policy;
    monsun_interlock_t interlocks[MONSUN_MAX_INTERLOCKS];
    monsun_slot_t slots[MONSUN_MAX_SLOTS];
    monsun_level_ctrl_t level;
} monsun_pump_cfg_t;

typedef struct {
    monsun_pump_cfg_t cfg;
    monsun_mode_t mode;
    bool running;
    bool level_demand;
    monsun_fault_t fault;
    uint32_t run_started_s;
    uint32_t off_since_s;
    bool ever_stopped;        /* min_off_s only applies after the first stop */
} monsun_pump_t;

void monsun_pump_init(monsun_pump_t *p, const monsun_pump_cfg_t *cfg);
void monsun_pump_set_mode(monsun_pump_t *p, monsun_mode_t mode);
void monsun_pump_reset_fault(monsun_pump_t *p);

/* Evaluate one control step. Returns the desired output state (true = ON). Pure logic, no hardware. */
bool monsun_pump_step(monsun_pump_t *p, const monsun_input_t *inputs, size_t n_inputs,
                      const monsun_clock_t *clk);

#ifdef __cplusplus
}
#endif

#endif
