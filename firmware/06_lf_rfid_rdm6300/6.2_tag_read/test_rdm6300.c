#include "rdm6300_frame_parser.h"

#include "../6.3_rdm6300_payload_decode/rdm6300_payload_decoder.h"
#include "../6.4_signature_storage/rdm6300_signature_record.h"

#include <assert.h>
#include <string.h>

static const uint8_t valid_frame[] = {
    0x02, '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '1', 0x03
};

static void expect_parse_error(
    const uint8_t *bytes,
    size_t length,
    rdm6300_error_t expected)
{
    rdm6300_frame_t frame;
    rdm6300_error_t error = RDM6300_ERROR_NONE;

    assert(rdm6300_parse_frame(bytes, length, &frame, &error) ==
           RDM6300_PARSE_ERROR);
    assert(error == expected);
}

static void test_valid_frame(void)
{
    rdm6300_frame_t frame;
    rdm6300_payload_t payload;
    rdm6300_signature_record_t record;
    rdm6300_error_t error = RDM6300_ERROR_NONE;

    assert(rdm6300_parse_frame(
               valid_frame, sizeof(valid_frame), &frame, &error) ==
           RDM6300_PARSE_FRAME_READY);
    assert(error == RDM6300_ERROR_NONE);
    assert(frame.payload[0] == 0x01u);
    assert(frame.payload[4] == 0x05u);
    assert(frame.checksum == 0x01u);

    assert(rdm6300_decode_payload(frame.payload, &payload) ==
           RDM6300_PAYLOAD_OK);
    assert(payload.version_customer == 0x01u);
    assert(payload.unique_id == 0x02030405u);
    assert(strcmp(payload.unique_id_hex, "02030405") == 0);

    rdm6300_signature_record_init(&record, &frame, &payload, 0);
    assert(memcmp(record.raw_frame, valid_frame, sizeof(valid_frame)) == 0);
    assert(record.unique_id == 0x02030405u);
    assert(record.protocol == RFID_PROTOCOL_RDM6300_EM4100);
    assert(record.checksum_valid == 1u);
    assert(record.epoch_or_0 == 0u);
}

static void test_fragmented_input(void)
{
    rdm6300_parser_t parser;
    rdm6300_frame_t frame;
    rdm6300_error_t error = RDM6300_ERROR_NONE;
    size_t index;

    rdm6300_parser_init(&parser);
    for (index = 0; index < sizeof(valid_frame) - 1; index++) {
        assert(rdm6300_parser_push(
                   &parser, valid_frame[index], &frame, &error) ==
               RDM6300_PARSE_NEED_MORE);
        assert(error == RDM6300_ERROR_NONE);
    }
    assert(rdm6300_parser_push(
               &parser, valid_frame[sizeof(valid_frame) - 1], &frame, &error) ==
           RDM6300_PARSE_FRAME_READY);
    assert(frame.payload[1] == 0x02u);
}

static void test_rejection_errors(void)
{
    uint8_t invalid_hex[sizeof(valid_frame)];
    uint8_t bad_stx[sizeof(valid_frame)];
    uint8_t bad_etx[sizeof(valid_frame)];
    uint8_t bad_checksum[sizeof(valid_frame)];

    memcpy(invalid_hex, valid_frame, sizeof(valid_frame));
    invalid_hex[4] = 'X';
    expect_parse_error(
        invalid_hex, sizeof(invalid_hex), RDM6300_ERROR_INVALID_HEX);

    memcpy(bad_stx, valid_frame, sizeof(valid_frame));
    bad_stx[0] = 0x01;
    expect_parse_error(bad_stx, sizeof(bad_stx), RDM6300_ERROR_BAD_STX);

    memcpy(bad_etx, valid_frame, sizeof(valid_frame));
    bad_etx[13] = 0x04;
    expect_parse_error(bad_etx, sizeof(bad_etx), RDM6300_ERROR_BAD_ETX);

    memcpy(bad_checksum, valid_frame, sizeof(valid_frame));
    bad_checksum[12] = '2';
    expect_parse_error(
        bad_checksum, sizeof(bad_checksum), RDM6300_ERROR_CHECKSUM_MISMATCH);

    expect_parse_error(
        valid_frame, sizeof(valid_frame) - 1, RDM6300_ERROR_WRONG_LENGTH);
}

static void test_stream_ignores_noise_before_frame(void)
{
    rdm6300_parser_t parser;
    rdm6300_frame_t frame;
    rdm6300_error_t error = RDM6300_ERROR_NONE;
    size_t index;

    rdm6300_parser_init(&parser);
    assert(rdm6300_parser_push(&parser, 0x00, &frame, &error) ==
           RDM6300_PARSE_NEED_MORE);
    assert(error == RDM6300_ERROR_NONE);
    for (index = 0; index < sizeof(valid_frame); index++) {
        rdm6300_parser_push(&parser, valid_frame[index], &frame, &error);
    }
    assert(frame.payload[0] == 0x01u);
}

int main(void)
{
    test_valid_frame();
    test_fragmented_input();
    test_rejection_errors();
    test_stream_ignores_noise_before_frame();
    return 0;
}
