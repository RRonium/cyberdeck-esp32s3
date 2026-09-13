#include "rdm6300_signature_record.h"

#include <string.h>

void rdm6300_signature_record_init(
    rdm6300_signature_record_t *record,
    const rdm6300_frame_t *frame,
    const rdm6300_payload_t *payload,
    uint32_t epoch_or_0)
{
    if (record == NULL || frame == NULL || payload == NULL) {
        return;
    }

    memcpy(record->raw_frame, frame->raw, RDM6300_FRAME_LENGTH);
    record->unique_id = payload->unique_id;
    record->protocol = RFID_PROTOCOL_RDM6300_EM4100;
    record->checksum_valid = 1u;
    record->epoch_or_0 = epoch_or_0;
}
