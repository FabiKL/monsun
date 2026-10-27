# Roadmap

- **M0** Concept docs, repo scaffold, host-tested engine and link libs.
- **M1** Controller on XIAO-C3 for the IBC tank prototype, end-to-end in HA. Prove stand-alone behavior (WiFi off, schedules and level control still run).
- **M2** MonsunLink and first STM32 node (NUCLEO-G431RB), Python node simulator.
- **M3** More nodes (second STM32 family, ESP mini node), third-party Modbus profiles, enclosure and mains wiring.
- **M4** ESP32-C6 swap, optional Zigbee transport and Z2M converter generator, optional DS3231 RTC.

## Prototype pin plan (XIAO ESP32-C3)
| Function | Pin (GPIO) | Note |
|---|---|---|
| I²C SDA/SCL (ADS1115) | D4/D5 (6/7) | |
| RS485 TX/RX (later) | D6/D7 (21/20) | UART0 |
| Relay 1/2 | D1/D2 (3/4) | Non-strapping; add pull-ups so relays stay off at boot |
| Digital levels 1/2 | D3/D10 (5/10) | Internal pull-up, switch to GND |
| DHT22 | D0 (2) | Strapping pin, must be high at boot: the DHT pull-up does that |
| 1-Wire DS18B20 | D8 (8) | Strapping pin, high at boot via the 4.7 kΩ pull-up |
| D9 (9) | unused | BOOT button |

The pin table is provisional. Verify against the XIAO ESP32-C3 pinout and keep it in `firmware/controller/boards/xiao_esp32c3.h`.
