#include "rdm6300_frame_parser.h"

#include <string.h>

static int hex_value(uint8_t character)
{
    if (character >= '0' && character <= '9') {
        return (int)(character - '0');
    }
    if (character >= 'A' && character <= 'F') {
        return (int)(character - 'A') + 10;
    }
    if (character >= 'a' && character <= 'f') {
        return (int)(character - 'a') + 10;
    }
    return -1;
}

static void set_error(rdm6300_error_t *error, rdm6300_error_t value)
{
    if (error != NULL) {
        *error = value;
    }
}

static rdm6300_parse_result_t fail(
    rdm6300_parser_t *parser,
    rdm6300_error_t *error,
    rdm6300_error_t value)
{
    parser->length = 0;
    set_error(error, value);
    return RDM6300_PARSE_ERROR;
}

void rdm6300_parser_init(rdm6300_parser_t *parser)
{
    if (parser != NULL) {
        parser->length = 0;
    }
}

rdm6300_parse_result_t rdm6300_parse_frame(
    const uint8_t *bytes,
    size_t length,
    rdm6300_frame_t *frame,
    rdm6300_error_t *error)
{
    uint8_t checksum = 0;
    size_t index;

    set_error(error, RDM6300_ERROR_NONE);
    if (bytes == NULL || frame == NULL || length != RDM6300_FRAME_LENGTH) {
        set_error(error, RDM6300_ERROR_WRONG_LENGTH);
        return RDM6300_PARSE_ERROR;
    }
    if (bytes[0] != 0x02u) {
        set_error(error, RDM6300_ERROR_BAD_STX);
        return RDM6300_PARSE_ERROR;
    }
    if (bytes[13] != 0x03u) {
        set_error(error, RDM6300_ERROR_BAD_ETX);
        return RDM6300_PARSE_ERROR;
    }

    for (index = 1; index < 13; index++) {
        if (hex_value(bytes[index]) < 0) {
            set_error(error, RDM6300_ERROR_INVALID_HEX);
            return RDM6300_PARSE_ERROR;
        }
    }
    for (index = 0; index < RDM6300_PAYLOAD_LENGTH; index++) {
        checksum ^= (uint8_t)(
            (hex_value(bytes[1 + index * 2]) << 4) |
            hex_value(bytes[2 + index * 2]));
        frame->payload[index] = (uint8_t)(
            (hex_value(bytes[1 + index * 2]) << 4) |
            hex_value(bytes[2 + index * 2]));
    }
    frame->checksum = (uint8_t)(
        (hex_value(bytes[11]) << 4) | hex_value(bytes[12]));
    if (checksum != frame->checksum) {
        set_error(error, RDM6300_ERROR_CHECKSUM_MISMATCH);
        return RDM6300_PARSE_ERROR;
    }

    memcpy(frame->raw, bytes, RDM6300_FRAME_LENGTH);
    return RDM6300_PARSE_FRAME_READY;
}

rdm6300_parse_result_t rdm6300_parser_push(
    rdm6300_parser_t *parser,
    uint8_t byte,
    rdm6300_frame_t *frame,
    rdm6300_error_t *error)
{
    if (parser == NULL || frame == NULL) {
        set_error(error, RDM6300_ERROR_WRONG_LENGTH);
        return RDM6300_PARSE_ERROR;
    }
    if (parser->length == 0 && byte != 0x02u) {
        set_error(error, RDM6300_ERROR_NONE);
        return RDM6300_PARSE_NEED_MORE;
    }
    if (parser->length >= RDM6300_FRAME_LENGTH) {
        return fail(parser, error, RDM6300_ERROR_WRONG_LENGTH);
    }

    parser->buffer[parser->length++] = byte;
    if (parser->length < RDM6300_FRAME_LENGTH) {
        set_error(error, RDM6300_ERROR_NONE);
        return RDM6300_PARSE_NEED_MORE;
    }

    {
        rdm6300_parse_result_t result = rdm6300_parse_frame(
            parser->buffer, parser->length, frame, error);
        parser->length = 0;
        return result;
    }
}

const char *rdm6300_error_string(rdm6300_error_t error)
{
    switch (error) {
    case RDM6300_ERROR_NONE:
        return "none";
    case RDM6300_ERROR_WRONG_LENGTH:
        return "wrong length";
    case RDM6300_ERROR_INVALID_HEX:
        return "invalid hexadecimal character";
    case RDM6300_ERROR_BAD_STX:
        return "bad start sentinel";
    case RDM6300_ERROR_BAD_ETX:
        return "bad end sentinel";
    case RDM6300_ERROR_CHECKSUM_MISMATCH:
        return "checksum mismatch";
    }
    return "unknown error";
}
