# Bill of materials (first prototype, dev-kits and modules)

## Controller
| Item | Status | Notes |
|---|---|---|
| Seeed XIAO ESP32-C3 | owned | Later: ESP32-C6-DevKitC-1 (on order) |
| ADS1115 16-bit ADC I²C module | to buy | Reads the 5 PSI sensor |
| 10k / 15k resistors | to buy | Divider: 4.5 V → 2.7 V |
| 5 PSI 0.5–4.5 V pressure sensor (5 V supply) | owned | Ratiometric; keep 5 V stable |
| AM2302 (DHT22) | owned | Air temperature/humidity |
| DS18B20 waterproof + 4.7 kΩ | to buy | Water temperature |
| 2-channel opto relay module (10 A, 3.3 V-trigger capable) | to buy | Check active-low and JD-VCC |
| 2x float switch / digital level sensor | owned | Reservoir not empty, IBC full |
| 5 V PSU (e.g. DIN or encapsulated) + 24 V PSU | to buy | |
| Enclosure IP65+, DIN rail, terminals, glands | to buy | |
| RCD, MCB/fuses, PE terminals | to buy | See [safety.md](safety.md) |

## Bus node 1 (M2)
| Item | Status |
|---|---|
| NUCLEO-G431RB (or G474RE) | owned |
| 3.3 V RS485 module (MAX3485/SP3485) x2 | to buy |
| USB–RS485 adapter | to buy (PC test) |
| 24 V → 5 V DC/DC (wide-input) | to buy |
| 120 Ω termination resistors | to buy |

## Owned boards, planned use
NUCLEO-G431RB/G474RE/G491RE/G0B1RE/L152RE: nodes. NUCLEO-U575ZI, F746ZG: heavy I/O nodes later. P-NUCLEO-WB55: future Zigbee/BLE
sensor node. CAN FD shield: alternative bus experiment. SX1261MB2xAS: future wireless link. X-NUCLEO-IDS01A4: check datasheet
before planning use. LPCXpresso55S28, TFT shields: unused.
