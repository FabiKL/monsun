#ifndef BOARD_ESP32C6_DEVKITC1_H
#define BOARD_ESP32C6_DEVKITC1_H

/* ESP32-C6-DevKitC-1 pin map (GPIO numbers). Provisional, verify before wiring.
 * Avoid strapping pins GPIO4, 5, 8, 9, 15 and the USB pins GPIO12/13. */
#define MONSUN_PIN_I2C_SDA      6
#define MONSUN_PIN_I2C_SCL      7
#define MONSUN_PIN_RS485_TX     20
#define MONSUN_PIN_RS485_RX     21
#define MONSUN_PIN_RELAY_1      10
#define MONSUN_PIN_RELAY_2      11
#define MONSUN_PIN_LEVEL_1      2
#define MONSUN_PIN_LEVEL_2      3
#define MONSUN_PIN_DHT22        1
#define MONSUN_PIN_ONEWIRE      0

#endif
