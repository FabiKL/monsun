#ifndef MONSUN_LINK_H
#define MONSUN_LINK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MONSUN_LINK_MAGIC            0x4D53u
#define MONSUN_LINK_PROTO_VERSION    0x0000u
#define MONSUN_LINK_REG_MAGIC        0x0000u
#define MONSUN_LINK_REG_PROTO        0x0001u
#define MONSUN_LINK_REG_DEVTYPE      0x0002u
#define MONSUN_LINK_REG_FW_HI        0x0003u
#define MONSUN_LINK_REG_FW_LO        0x0004u
#define MONSUN_LINK_REG_ENTITY_COUNT 0x0005u
#define MONSUN_LINK_REG_FAILSAFE_S   0x0006u
#define MONSUN_LINK_REG_DESC_BASE    0x0010u
#define MONSUN_LINK_DESC_REGS        8u
#define MONSUN_LINK_MAX_PDU          256u

/* Modbus function codes used by MonsunLink */
#define MB_FC_READ_COILS        0x01u
#define MB_FC_READ_HOLDING      0x03u
#define MB_FC_READ_INPUT        0x04u
#define MB_FC_WRITE_COIL        0x05u
#define MB_FC_WRITE_REG         0x06u

typedef enum {
    MONSUN_ENT_SENSOR = 1,
    MONSUN_ENT_BINARY_SENSOR = 2,
    MONSUN_ENT_SWITCH = 3,
    MONSUN_ENT_NUMBER = 4,
} monsun_entity_type_t;

typedef enum {
    MONSUN_REG_COIL = 0,
    MONSUN_REG_DISCRETE = 1,
    MONSUN_REG_INPUT = 2,
    MONSUN_REG_HOLDING = 3,
} monsun_reg_kind_t;

#define MONSUN_DESC_FLAG_WRITABLE   0x0001u
#define MONSUN_DESC_FLAG_DIAGNOSTIC 0x0002u

typedef struct {
    uint16_t entity_type;
    uint16_t device_class;
    uint16_t unit;
    uint16_t reg_addr;
    uint16_t reg_kind;
    int16_t  scale; /* value = raw * 10^scale */
    uint16_t flags;
    uint16_t name_id;
} monsun_entity_desc_t;

/* CRC16/MODBUS (poly 0xA001, init 0xFFFF) */
uint16_t monsun_crc16(const uint8_t *data, size_t len);

/* Build a request frame including slave address and CRC. Returns frame length or 0 on error. */
size_t monsun_build_read_request(uint8_t *out, size_t out_size, uint8_t slave, uint8_t fc,
                                 uint16_t start, uint16_t count);
size_t monsun_build_write_reg_request(uint8_t *out, size_t out_size, uint8_t slave,
                                      uint16_t reg, uint16_t value);

/* Validate a frame (length >= 4 and CRC ok) and return pointer to PDU (function code first).
 * *slave receives the address and *pdu_len the PDU length. Returns NULL if invalid. */
const uint8_t *monsun_parse_frame(const uint8_t *frame, size_t len, uint8_t *slave, size_t *pdu_len);

/* Parse a read-registers response PDU into host-order registers. Returns count or -1. */
int monsun_parse_read_regs_response(const uint8_t *pdu, size_t pdu_len, uint16_t *regs, size_t max_regs);

/* Parse the 8 register descriptor block into a struct. */
void monsun_desc_from_regs(const uint16_t regs[MONSUN_LINK_DESC_REGS], monsun_entity_desc_t *out);
void monsun_desc_to_regs(const monsun_entity_desc_t *desc, uint16_t regs[MONSUN_LINK_DESC_REGS]);

/* Check the header registers 0x0000..0x0005 of a node. */
bool monsun_header_valid(const uint16_t header[6]);

#ifdef __cplusplus
}
#endif

#endif
