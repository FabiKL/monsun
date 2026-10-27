# ADR-0003: MonsunLink uses RS485 with Modbus RTU framing

Status: accepted (draft details)

## Context
Extensible bus over a 4-wire cable (24 V, GND, A, B), preferably with third-party sensor support.

## Decision
RS485 half-duplex, Modbus RTU framing, plus Monsun self-description registers so nodes register their own entities.

## Alternatives
- Custom protocol: more flexible (events), but no third-party devices and more work.
- CAN FD: robust, but needs more cable pairs/transceivers; may be added as a physical layer later.

## Consequences
- Master polling only; no events. 1 Hz polling is enough for irrigation.
- Cheap RS485 modules and many industrial sensors work.
