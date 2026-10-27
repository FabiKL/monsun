# ADR-0002: WiFi + MQTT discovery as primary Home Assistant transport

Status: accepted

## Context
The controller should be one HA device with many entities, rich configuration (schedules, thresholds) and be developed on a chip without 802.15.4.

## Decision
Use WiFi + MQTT with HA discovery on the existing Mosquitto broker. Zigbee is an optional later transport on ESP32-C6.

## Alternatives
- **Zigbee via Z2M:** needs a per-device external converter, custom clusters for schedules, and a C6. Good for battery sensors, which stay direct-to-HA.
- **ESPHome native API:** less effort but less customizable; ESPHome is used for small direct sensors only.

## Consequences
- Depends on WiFi range in the garden (external antenna or AP).
- Standalone logic on the device, so WiFi loss does not stop watering.
