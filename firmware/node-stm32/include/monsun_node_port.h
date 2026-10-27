#ifndef MONSUN_NODE_PORT_H
#define MONSUN_NODE_PORT_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* Interface every STM32 family port implements. The portable node logic only uses these functions. */

#ifdef __cplusplus
extern "C" {
#endif

/* Millisecond tick since boot. */
uint32_t monsun_port_millis(void);

/* Transmit bytes on the RS485 UART. Implementations must switch the driver enable (DE/RE) around the transmission
 * unless the transceiver module handles direction automatically. Returns after the last bit has been sent. */
bool monsun_port_uart_write(const uint8_t *data, size_t len);

/* Called from the UART receive path for every received byte. */
void monsun_port_uart_rx_byte(uint8_t byte);

/* Persistent storage for the node address and settings. */
bool monsun_port_storage_read(uint8_t *data, size_t len);
bool monsun_port_storage_write(const uint8_t *data, size_t len);

/* Drive all outputs to their safe state. Called on boot, watchdog timeout and fail-safe timeout. */
void monsun_port_outputs_safe(void);

#ifdef __cplusplus
}
#endif

#endif
