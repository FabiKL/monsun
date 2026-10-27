#include "monsun_link.h"

#include <string.h>

uint16_t monsun_crc16(const uint8_t *data, size_t len)
{
    uint16_t crc = 0xFFFFu;
    for (size_t i = 0; i < len; i++) {
        crc ^= data[i];
        for (int b = 0; b < 8; b++) {
            crc = (crc & 1u) ? (uint16_t)((crc >> 1) ^ 0xA001u) : (uint16_t)(crc >> 1);
        }
    }
    return crc;
}

static size_t finish_frame(uint8_t *out, size_t len)
{
    uint16_t crc = monsun_crc16(out, len);
    out[len] = (uint8_t)(crc & 0xFFu); /* Modbus RTU sends CRC low byte first */
    out[len + 1] = (uint8_t)(crc >> 8);
    return len + 2;
}

size_t monsun_build_read_request(uint8_t *out, size_t out_size, uint8_t slave, uint8_t fc,
                                 uint16_t start, uint16_t count)
{
    if (out == NULL || out_size < 8 || count == 0) {
        return 0;
    }
    out[0] = slave;
    out[1] = fc;
    out[2] = (uint8_t)(start >> 8);
    out[3] = (uint8_t)(start & 0xFFu);
    out[4] = (uint8_t)(count >> 8);
    out[5] = (uint8_t)(count & 0xFFu);
    return finish_frame(out, 6);
}

size_t monsun_build_write_reg_request(uint8_t *out, size_t out_size, uint8_t slave,
                                      uint16_t reg, uint16_t value)
{
    if (out == NULL || out_size < 8) {
        return 0;
    }
    out[0] = slave;
    out[1] = MB_FC_WRITE_REG;
    out[2] = (uint8_t)(reg >> 8);
    out[3] = (uint8_t)(reg & 0xFFu);
    out[4] = (uint8_t)(value >> 8);
    out[5] = (uint8_t)(value & 0xFFu);
    return finish_frame(out, 6);
}

const uint8_t *monsun_parse_frame(const uint8_t *frame, size_t len, uint8_t *slave, size_t *pdu_len)
{
    if (frame == NULL || len < 4 || len > MONSUN_LINK_MAX_PDU + 3) {
        return NULL;
    }
    uint16_t expected = monsun_crc16(frame, len - 2);
    uint16_t got = (uint16_t)(frame[len - 2] | (frame[len - 1] << 8));
    if (expected != got) {
        return NULL;
    }
    if (slave != NULL) {
        *slave = frame[0];
    }
    if (pdu_len != NULL) {
        *pdu_len = len - 3;
    }
    return &frame[1];
}

int monsun_parse_read_regs_response(const uint8_t *pdu, size_t pdu_len, uint16_t *regs, size_t max_regs)
{
    if (pdu == NULL || pdu_len < 2 || (pdu[0] & 0x80u) != 0) {
        return -1;
    }
    if (pdu[0] != MB_FC_READ_HOLDING && pdu[0] != MB_FC_READ_INPUT) {
        return -1;
    }
    size_t bytes = pdu[1];
    if ((bytes & 1u) != 0 || pdu_len != bytes + 2u || bytes / 2u > max_regs) {
        return -1;
    }
    for (size_t i = 0; i < bytes / 2u; i++) {
        regs[i] = (uint16_t)((pdu[2 + 2 * i] << 8) | pdu[3 + 2 * i]);
    }
    return (int)(bytes / 2u);
}

void monsun_desc_from_regs(const uint16_t regs[MONSUN_LINK_DESC_REGS], monsun_entity_desc_t *out)
{
    out->entity_type = regs[0];
    out->device_class = regs[1];
    out->unit = regs[2];
    out->reg_addr = regs[3];
    out->reg_kind = regs[4];
    out->scale = (int16_t)regs[5];
    out->flags = regs[6];
    out->name_id = regs[7];
}

void monsun_desc_to_regs(const monsun_entity_desc_t *desc, uint16_t regs[MONSUN_LINK_DESC_REGS])
{
    regs[0] = desc->entity_type;
    regs[1] = desc->device_class;
    regs[2] = desc->unit;
    regs[3] = desc->reg_addr;
    regs[4] = desc->reg_kind;
    regs[5] = (uint16_t)desc->scale;
    regs[6] = desc->flags;
    regs[7] = desc->name_id;
}

bool monsun_header_valid(const uint16_t header[6])
{
    return header[0] == MONSUN_LINK_MAGIC && (header[1] >> 8) == (MONSUN_LINK_PROTO_VERSION >> 8) &&
           header[5] <= 64u;
}
