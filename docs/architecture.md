# Architecture

```
            Home Assistant ── MQTT (Mosquitto, shared with Z2M)
                   │  discovery + state + config
        ┌──────────┴───────────────────────────────┐
        │ Controller (ESP32)                       │
        │  monsun_mqtt   monsun_registry           │
        │  monsun_engine (schedules, level, safety)│
        │  monsun_io (local GPIO/I²C/1-Wire)       │
        │  monsun_link_master (Modbus RTU master)  │
        └──────────┬───────────────────────────────┘
                   │ RS485 (24V, GND, A, B)  = MonsunLink
        ┌──────────┴──────┬──────────────┐
     STM32 node       STM32 node      ESP mini node
```

## Components
| Component | Location | Notes |
|---|---|---|
| `monsun_engine` | `firmware/libs/monsun_engine` | Portable C, no hardware access, unit-tested on host. |
| `monsun_link` | `firmware/libs/monsun_link` | Portable C: CRC, Modbus RTU frames, descriptor parse/build. Used by controller (master) and nodes (slave). |
| Controller | `firmware/controller` | ESP-IDF app plus components. |
| STM32 nodes | `firmware/node-stm32` | CMake + Cube HAL, one port per family. |

## Entity registry
Every sensor, actuator and config value is an *entity* with: id, type (`sensor`, `binary_sensor`, `switch`, `number`,
`select`, `time`), device class, unit, min/max/step, access (R/W), and a source (local driver, bus node register,
remote MQTT input). The MQTT layer publishes HA discovery from the registry; the engine reads/writes via the registry.

## Engine
- Pure function style: `monsun_engine_step(state, inputs, now) -> outputs`.
- Per actuator: mode (`off|auto|on`), max run time, min off time, interlocks.
- Programs: timer slots and level control with hysteresis.
- Faults latch until reset from HA (dry-run, sensor stale, max runtime).
- Inputs carry a *validity/staleness* flag; invalid safety input ⇒ outputs off.

## Stand-alone behavior
- Config persisted in NVS; HA edits are applied and echoed back.
- No dependency on network for control decisions.
- Time: SNTP, HA set, last-known epoch saved in NVS; `time_valid` diagnostic entity.

## Transports
`monsun_mqtt` is the primary transport. A `transport` interface (publish entity state, receive writes) allows a later
`monsun_zigbee` backend on ESP32-C6.

## Fail-safe rules
- Outputs OFF at boot until the engine has run once.
- Task watchdog on the engine task.
- Nodes turn outputs OFF if no valid master frame arrives within the configured timeout.
