# ADR-0001: ESP32 controller, start on XIAO ESP32-C3, move to ESP32-C6

Status: accepted

## Context
A controller with WiFi, enough RAM for schedules and MQTT, and optional Zigbee. Owned: XIAO ESP32-C3. ESP32-C6-DevKitC-1 has a 3 week lead time.

## Decision
Develop on the XIAO ESP32-C3 (RISC-V, WiFi). Switch to the C6 (WiFi 6 + 802.15.4) by changing the IDF target and the board pin map.
Keep Zigbee code behind a transport interface.

## Consequences
- The C3 has only ~11 GPIOs. I²C and the RS485 bus carry further I/O.
- Zigbee cannot be tested until the C6 arrives.
- STM32 and Raspberry Pi are not used as the main controller.
