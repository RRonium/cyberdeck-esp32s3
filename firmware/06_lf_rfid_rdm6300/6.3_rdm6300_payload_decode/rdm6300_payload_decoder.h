#ifndef RDM6300_PAYLOAD_DECODER_H
#define RDM6300_PAYLOAD_DECODER_H

#include <stdint.h>

#define RDM6300_ID_HEX_LENGTH 8u

typedef enum {
    RDM6300_PAYLOAD_OK = 0,
    RDM6300_PAYLOAD_INVALID_INPUT
} rdm6300_payload_result_t;

typedef struct {
    uint8_t version_customer;
    uint32_t unique_id;
    char unique_id_hex[RDM6300_ID_HEX_LENGTH + 1u];
} rdm6300_payload_t;

rdm6300_payload_result_t rdm6300_decode_payload(
    const uint8_t payload[5],
    rdm6300_payload_t *decoded);

#endif
