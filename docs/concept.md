# Monsun – Concept

Monsun is a modular, open-source garden watering and tank controller. One ESP32 **controller** is the brain.
It appears in Home Assistant (HA) as **one device** with all actuators and sensors, and works **stand-alone**
(schedules, level control and interlocks run on the device). Extra I/O is added through a 4-wire RS485 bus
(**MonsunLink**) using STM32 or ESP boards. The first build uses only dev-kits, modules and shields.

## Goals
- Control multiple pumps and valves (24 V and 230 V).
- Measure temperature, water level, soil moisture, flow, and more.
- One HA device with all entities, managed through HA control pages.
- Keep working when WiFi/MQTT/HA is down.
- Highly customizable and easy to extend: add a node and its entities appear in HA.
- Built from widely available modules (Arduino, Seeed Grove, Raspberry Pi, AliExpress/Amazon ecosystems).

## Non-goals (for now)
- Certified mains products. 230 V wiring needs qualified review (see [safety.md](safety.md)).
- Battery-powered sensor firmware (use Zigbee/ESPHome devices directly in HA).
- Custom PCBs in the first build.

## Three tiers
1. **Controller** (ESP32): engine, local I/O, MonsunLink master, MQTT/HA.
2. **Bus nodes** (STM32 Nucleo or ESP mini + RS485 transceiver): wired I/O, self-describing, fail-safe.
3. **Direct-to-HA devices** (ESPHome minis, Zigbee sensors): independent, never needed for safety.

See [architecture.md](architecture.md) and [sensor-placement.md](sensor-placement.md).

## Key decisions
See [adr/](adr/). Summary:

| Topic | Decision |
|---|---|
| Controller | ESP32, start on XIAO ESP32-C3, then ESP32-C6-DevKitC-1 |
| HA link | WiFi + MQTT discovery; Zigbee optional later |
| Firmware | ESP-IDF (controller), STM32Cube HAL per family (nodes), portable C core libs |
| Bus | RS485, Modbus RTU framing + Monsun self-description |
| Time | SNTP/HA, last-known time in NVS, no RTC in prototype |

## First prototype: IBC tank controller
Two 230 V submersible pumps (reservoir pump, IBC pump), two digital level switches, one analog 5 PSI (0.5–4.5 V)
pressure sensor for IBC level, air and water temperature. See [roadmap.md](roadmap.md) and [bom.md](bom.md).
