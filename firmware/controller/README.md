# Monsun controller (ESP-IDF)

Target: XIAO ESP32-C3 now, ESP32-C6-DevKitC-1 later. Tested with ESP-IDF v5.5.1.

```
cd firmware/controller
idf.py build flash monitor
# C6: idf.py set-target esp32c6 && idf.py build
```

## Current state
Builds for `esp32c3`. `main.c` wires the engine to the local float switches and relay outputs for the IBC tank
prototype. The analog IBC level is not implemented yet and stays *invalid*, so pumps stay off (fail-safe).
Pin maps live in `boards/`; component plan in `components/README.md`.

**Do not connect mains loads before reading [docs/safety.md](../../docs/safety.md).**
