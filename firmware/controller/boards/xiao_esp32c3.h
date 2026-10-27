#ifndef BOARD_XIAO_ESP32C3_H
#define BOARD_XIAO_ESP32C3_H

/* Seeed XIAO ESP32-C3 pin map (GPIO numbers). Provisional, see docs/roadmap.md. */
#define MONSUN_PIN_I2C_SDA      6   /* D4 */
#define MONSUN_PIN_I2C_SCL      7   /* D5 */
#define MONSUN_PIN_RS485_TX     21  /* D6 */
#define MONSUN_PIN_RS485_RX     20  /* D7 */
#define MONSUN_PIN_RELAY_1      3   /* D1 */
#define MONSUN_PIN_RELAY_2      4   /* D2 */
#define MONSUN_PIN_LEVEL_1      5   /* D3: reservoir not empty */
#define MONSUN_PIN_LEVEL_2      10  /* D10: IBC full */
#define MONSUN_PIN_DHT22        2   /* D0, strapping pin: high at boot via DHT pull-up */
#define MONSUN_PIN_ONEWIRE      8   /* D8, strapping pin: high at boot via 4.7k pull-up */

#endif
