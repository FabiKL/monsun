# Controller components (planned)

| Component | Purpose | Status |
|---|---|---|
| `monsun_libs` | Wraps the portable libs from `firmware/libs` | done |
| `monsun_registry` | Entity registry | basic |
| `monsun_io` | Local drivers: GPIO, ADS1115, DHT22, DS18B20 | planned (M1) |
| `monsun_time` | SNTP, set-from-HA, NVS last-known time | planned (M1) |
| `monsun_config` | NVS persistence of pump/schedule config | planned (M1) |
| `monsun_mqtt` | MQTT client and HA discovery | planned (M1) |
| `monsun_link_master` | Modbus RTU master, node scan | planned (M2) |

Add each component as its own folder here, with its own `CMakeLists.txt`.
