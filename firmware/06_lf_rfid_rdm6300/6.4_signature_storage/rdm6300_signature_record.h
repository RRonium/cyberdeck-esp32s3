#ifndef RDM6300_SIGNATURE_RECORD_H
#define RDM6300_SIGNATURE_RECORD_H

#include <stdint.h>

#include "../6.2_tag_read/rdm6300_frame_parser.h"
#include "../6.3_rdm6300_payload_decode/rdm6300_payload_decoder.h"

typedef enum {
    RFID_PROTOCOL_UNKNOWN = 0,
    RFID_PROTOCOL_RDM6300_EM4100 = 1
} rfid_protocol_t;

typedef struct {
    uint8_t raw_frame[RDM6300_FRAME_LENGTH];
    uint32_t unique_id;
    rfid_protocol_t protocol;
    uint8_t checksum_valid;
    uint32_t epoch_or_0;
} rdm6300_signature_record_t;

void rdm6300_signature_record_init(
    rdm6300_signature_record_t *record,
    const rdm6300_frame_t *frame,
    const rdm6300_payload_t *payload,
    uint32_t epoch_or_0);

#endif
