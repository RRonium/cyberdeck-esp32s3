#include "rdm6300_payload_decoder.h"

#include <stddef.h>

rdm6300_payload_result_t rdm6300_decode_payload(
    const uint8_t payload[5],
    rdm6300_payload_t *decoded)
{
    static const char hex[] = "0123456789ABCDEF";

    if (payload == NULL || decoded == NULL) {
        return RDM6300_PAYLOAD_INVALID_INPUT;
    }

    decoded->version_customer = payload[0];
    decoded->unique_id =
        ((uint32_t)payload[1] << 24) |
        ((uint32_t)payload[2] << 16) |
        ((uint32_t)payload[3] << 8) |
        (uint32_t)payload[4];
    for (size_t index = 0; index < 4; index++) {
        decoded->unique_id_hex[index * 2] =
            hex[(payload[index + 1] >> 4) & 0x0Fu];
        decoded->unique_id_hex[index * 2 + 1] =
            hex[payload[index + 1] & 0x0Fu];
    }
    decoded->unique_id_hex[RDM6300_ID_HEX_LENGTH] = '\0';
    return RDM6300_PAYLOAD_OK;
}
