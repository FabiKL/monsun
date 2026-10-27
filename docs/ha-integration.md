# Home Assistant integration

## Transport
MQTT with HA discovery, using the same Mosquitto broker as Zigbee2MQTT. The controller is **one HA device**
(`device.identifiers = ["monsun_<chipid>"]`) with all entities attached.

## Topics (draft)
```
monsun/<id>/availability           online|offline (LWT)
monsun/<id>/<entity>/state         current value
monsun/<id>/<entity>/set           write (switch/number/select/time)
homeassistant/<component>/monsun_<id>/<entity>/config   discovery (retained)
```

## Entities for the IBC prototype
| Entity | Type | Notes |
|---|---|---|
| Reservoir pump / IBC pump | `select` mode (off/auto/on) + `binary_sensor` running | |
| IBC level | `sensor` % and litres | tank geometry configurable |
| Reservoir not empty / IBC full | `binary_sensor` | |
| Air temperature, humidity, water temperature | `sensor` | |
| Target level high/low, max runtime, min off time | `number` | |
| Slot 1/2 per pump: enable, start time, duration, days preset | `switch`, `time`, `number`, `select` | |
| Fault code, time valid, WiFi RSSI, uptime | diagnostic | |
| Reset fault | `button` | |

## Config handling
The device is the source of truth (NVS). HA writes go to `/set` topics; the device validates, applies, persists and
republishes the state. Full JSON schedules and a custom card are planned for a later milestone.

## Zigbee (later)
An optional `monsun_zigbee` transport on ESP32-C6 plus a generated Zigbee2MQTT external converter.
Dashboards live in `homeassistant/dashboards/`.
