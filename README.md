# Monsun

Modular, open-source garden watering and tank controller built around an ESP32 and Home Assistant.

- One Home Assistant device with all pumps, valves and sensors (MQTT discovery).
- Stand-alone: schedules, level control and safety interlocks run on the device, with or without WiFi.
- Extensible through **MonsunLink**, a 4-wire RS485 bus (24 V, GND, A, B) with self-describing STM32/ESP nodes.
- Built from off-the-shelf dev-kits, modules and shields.

> **Status:** concept and scaffold. Nothing here is tested on hardware yet. Mains (230 V) wiring is dangerous. Read [docs/safety.md](docs/safety.md).

## Repository layout
| Path | Content |
|---|---|
| [docs/](docs/concept.md) | Concept, architecture, protocol, safety, BOM, ADRs |
| `firmware/libs/` | Portable C libraries (`monsun_engine`, `monsun_link`) |
| `firmware/controller/` | ESP-IDF controller firmware (ESP32-C3 now, ESP32-C6 later) |
| `firmware/node-stm32/` | STM32Cube HAL bus nodes |
| `firmware/tests/host/` | Host unit tests |
| `config/` | Example configs and third-party Modbus profiles |
| `tools/` | Node simulator and CLI helpers |
| `homeassistant/` | Dashboards, blueprints, later Zigbee2MQTT converter |
| `hardware/` | Wiring, bus cable, enclosure, BOM |

## First prototype
IBC tank controller: two 230 V pumps, two digital level switches, one analog pressure sensor, air and water temperature. See [docs/roadmap.md](docs/roadmap.md).

## Run the host tests
```
cmake -S firmware/tests/host -B build/host
cmake --build build/host
ctest --test-dir build/host --output-on-failure
```

## License
[MIT](LICENSE)
