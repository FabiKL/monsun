#ifndef BOARD_H
#define BOARD_H

#include "sdkconfig.h"

#if CONFIG_IDF_TARGET_ESP32C3
#include "xiao_esp32c3.h"
#elif CONFIG_IDF_TARGET_ESP32C6
#include "esp32c6_devkitc1.h"
#else
#error "No board pin map for this target"
#endif

/* Relay modules are usually active-low */
#define MONSUN_RELAY_ACTIVE_LEVEL 0
/* Float switch closes to GND when water is present (internal pull-up) */
#define MONSUN_LEVEL_ACTIVE_LEVEL 0

#endif
