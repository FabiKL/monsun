# MonsunLink (draft v0)

Physical and framing layer for extension nodes. Status: **draft**, will change during M2.

## Physical
- RS485, half-duplex, multi-drop. Default 19200 8N1 (configurable up to 115200).
- 4-wire cable: `24V`, `GND`, `A`, `B`. A/B on a twisted pair. Bus supply protected by a PTC fuse at the controller.
- Each node has a local DC/DC (24 V → 5 V / 3.3 V). Isolated RS485 where power domains differ.
- 120 Ω termination at both ends only; bias at the master.
- Recommended outdoor connector: M12 A-coded 4-pin (pinout in `hardware/bus-cable/`).

## Framing
Standard **Modbus RTU** (CRC16, function codes 0x03, 0x04, 0x06, 0x10, 0x05). Master = controller.
Addresses 1..247, stored in node flash, set via commissioning (default 247 = unconfigured).

## Monsun self-description registers (holding registers)
| Address | Content |
|---|---|
| 0x0000 | Magic `0x4D53` ("MS") |
| 0x0001 | Protocol version (major<<8 \| minor) |
| 0x0002 | Device type id |
| 0x0003–0x0004 | Firmware version (u32) |
| 0x0005 | Entity count N |
| 0x0006 | Fail-safe timeout in seconds (R/W) |
| 0x0010 + 8·i | Entity descriptor i (8 registers) |

### Entity descriptor (8 registers)
| Offset | Content |
|---|---|
| 0 | Entity type (1 sensor, 2 binary_sensor, 3 switch, 4 number) |
| 1 | Device class id |
| 2 | Unit id |
| 3 | Value register address |
| 4 | Value register kind (0 coil, 1 discrete, 2 input reg, 3 holding reg) |
| 5 | Scale (int16, value = raw × 10^scale) |
| 6 | Flags (bit0 writable, bit1 diagnostic) |
| 7 | Name id (index into a name table, or 0xFFFF for default) |

Ids are defined in `firmware/libs/monsun_link/include/monsun_link.h`.

## Master behavior
1. Scan addresses, read 0x0000..0x0005; accept only the magic.
2. Read descriptor table, register entities, publish HA discovery.
3. Poll values at 1 Hz (configurable). Timeouts mark entities unavailable and invalidate dependent inputs.

## Node fail-safe
If no valid request addressed to the node arrives within the fail-safe timeout, all outputs go to their safe state (OFF).

## Third-party Modbus devices
Described by YAML profiles in `config/profiles/`; they do not implement the descriptor block.
