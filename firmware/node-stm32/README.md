# Monsun STM32 nodes (STM32Cube HAL)

One port per MCU family. The portable core is `firmware/libs/monsun_link`; ports implement
[`include/monsun_node_port.h`](include/monsun_node_port.h) (tick, UART, storage, safe outputs).

| Port | Board | Notes | Status |
|---|---|---|---|
| `ports/g431rb` | NUCLEO-G431RB | First node. USART hardware driver-enable (DE) for RS485. | planned (M2) |
| `ports/l152re` | NUCLEO-L152RE | Proves portability. GPIO-controlled DE or auto-direction module. | planned (M2/M3) |

## Build (planned)
Fetch the Cube HAL package for the family into `third_party/` (git-ignored), for example
`https://github.com/STMicroelectronics/STM32CubeG4`, then:
```
cmake -S . -B build/g431rb -DCMAKE_TOOLCHAIN_FILE=cmake/arm-none-eabi.cmake -DMONSUN_PORT=g431rb
```
The `CMakeLists.txt` in this folder only declares the structure for now. No port builds yet.

## Hardware
Nucleo + 3.3 V RS485 module (MAX3485/SP3485), 24 V → 5 V DC/DC from the bus. Check the Nucleo power jumpers
(JP5/E5V on Nucleo-64) when feeding 5 V externally.
