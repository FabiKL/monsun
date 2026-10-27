# ADR-0004: STM32 nodes use STM32Cube HAL per family

Status: accepted

## Context
Owned boards span L1, G0, G4, WB, H5, U5 families. Zephyr would give one codebase, but the project prefers maximum control.

## Decision
STM32Cube HAL with CMake and arm-none-eabi-gcc, one small port per family. The protocol and node logic live in the portable
C library `monsun_link`. Start with G4 (hardware RS485 driver enable), then L1 to prove portability.

## Consequences
- More porting effort per family.
- Ports only provide UART, millisecond tick and flash storage.
