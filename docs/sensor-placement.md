# Sensor placement: bus vs. direct to Home Assistant

| Put it on the controller / MonsunLink | Put it directly into HA |
|---|---|
| Safety and interlock inputs (dry-run, overflow, min/max level) | Informational data (weather, soil spots, extra temperatures) |
| Inputs of control loops (tank level, pump current) | Battery powered or far from any cable (Zigbee soil sensors) |
| Needs deterministic latency and known staleness | Valuable stand-alone (device with own display/logic) |
| Pump drivers and relays | Mains-powered smart devices with their own app |

## Rule of thumb
If losing the value must stop a pump, it belongs on the controller or the bus.

## Remote tank sensor next to a 230 V pump (IBC example)
- **Cable possible:** MonsunLink node. Use an isolated RS485 transceiver and its own PSU; connect only A/B/GND.
- **Wireless only:** ESPHome/MQTT device that publishes to HA. Bind it to the controller as a *remote input* with
  a staleness timeout. A stale value is treated as unsafe and the dependent pump stops.
- A node with local logic (e.g. its own pump control) should keep working alone and only report telemetry.

## Remote inputs
A remote input maps an MQTT topic to a registry entity: `topic`, `value path`, `max_age_s`. Not allowed for the
only dry-run protection of a pump unless `max_age_s` is set.
