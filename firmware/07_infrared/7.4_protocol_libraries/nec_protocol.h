#ifndef NEC_PROTOCOL_H
#define NEC_PROTOCOL_H

#include <stddef.h>
#include <stdint.h>

#include "../7.2_pulse_learning/ir_pulse_train.h"

#define NEC_NORMAL_PULSE_COUNT 67u
#define NEC_REPEAT_PULSE_COUNT 3u

#define NEC_HEADER_MARK_US 9000u
#define NEC_HEADER_SPACE_US 4500u
#define NEC_REPEAT_SPACE_US 2250u
#define NEC_BIT_MARK_US 562u
#define NEC_ZERO_SPACE_US 562u
#define NEC_ONE_SPACE_US 1687u

#define NEC_HEADER_MARK_MIN_US 6750u
#define NEC_HEADER_MARK_MAX_US 11250u
#define NEC_HEADER_SPACE_MIN_US 3375u
#define NEC_HEADER_SPACE_MAX_US 5625u
#define NEC_REPEAT_SPACE_MIN_US 1687u
#define NEC_REPEAT_SPACE_MAX_US 2812u
#define NEC_BIT_MARK_MIN_US 422u
#define NEC_BIT_MARK_MAX_US 703u
#define NEC_ZERO_SPACE_MIN_US 422u
#define NEC_ZERO_SPACE_MAX_US 703u
#define NEC_ONE_SPACE_MIN_US 1265u
#define NEC_ONE_SPACE_MAX_US 2109u

typedef enum {
    NEC_FRAME_NORMAL = 0,
    NEC_FRAME_REPEAT = 1
} nec_frame_type_t;

typedef enum {
    NEC_OK = 0,
    NEC_ERROR_INVALID_ARGUMENT,
    NEC_ERROR_WRONG_PULSE_COUNT,
    NEC_ERROR_HEADER_MARK_OUT_OF_TOLERANCE,
    NEC_ERROR_HEADER_SPACE_OUT_OF_TOLERANCE,
    NEC_ERROR_REPEAT_SPACE_OUT_OF_TOLERANCE,
    NEC_ERROR_BIT_MARK_OUT_OF_TOLERANCE,
    NEC_ERROR_AMBIGUOUS_BIT,
    NEC_ERROR_COMPLEMENT_MISMATCH,
    NEC_ERROR_TERMINATING_MARK_OUT_OF_TOLERANCE,
    NEC_ERROR_OUTPUT_CAPACITY
} nec_error_t;

typedef struct {
    nec_frame_type_t type;
    uint8_t address;
    uint8_t command;
} nec_frame_t;

nec_error_t nec_decode(
    const ir_pulse_train_t *train,
    nec_frame_t *frame);

nec_error_t nec_encode(
    uint8_t address,
    uint8_t command,
    ir_pulse_train_t *train);

const char *nec_error_string(nec_error_t error);

#endif
