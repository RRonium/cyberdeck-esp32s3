#ifndef RDM6300_FRAME_PARSER_H
#define RDM6300_FRAME_PARSER_H

#include <stddef.h>
#include <stdint.h>

#define RDM6300_FRAME_LENGTH 14u
#define RDM6300_PAYLOAD_LENGTH 5u

typedef enum {
    RDM6300_ERROR_NONE = 0,
    RDM6300_ERROR_WRONG_LENGTH,
    RDM6300_ERROR_INVALID_HEX,
    RDM6300_ERROR_BAD_STX,
    RDM6300_ERROR_BAD_ETX,
    RDM6300_ERROR_CHECKSUM_MISMATCH
} rdm6300_error_t;

typedef enum {
    RDM6300_PARSE_NEED_MORE = 0,
    RDM6300_PARSE_FRAME_READY = 1,
    RDM6300_PARSE_ERROR = -1
} rdm6300_parse_result_t;

typedef struct {
    uint8_t raw[RDM6300_FRAME_LENGTH];
    uint8_t payload[RDM6300_PAYLOAD_LENGTH];
    uint8_t checksum;
} rdm6300_frame_t;

typedef struct {
    uint8_t buffer[RDM6300_FRAME_LENGTH];
    size_t length;
} rdm6300_parser_t;

void rdm6300_parser_init(rdm6300_parser_t *parser);

rdm6300_parse_result_t rdm6300_parser_push(
    rdm6300_parser_t *parser,
    uint8_t byte,
    rdm6300_frame_t *frame,
    rdm6300_error_t *error);

rdm6300_parse_result_t rdm6300_parse_frame(
    const uint8_t *bytes,
    size_t length,
    rdm6300_frame_t *frame,
    rdm6300_error_t *error);

const char *rdm6300_error_string(rdm6300_error_t error);

#endif
