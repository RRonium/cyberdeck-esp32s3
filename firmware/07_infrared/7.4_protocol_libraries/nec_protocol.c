#include "nec_protocol.h"

static int in_window(
    uint32_t value,
    uint32_t minimum,
    uint32_t maximum)
{
    return value >= minimum && value <= maximum;
}

static nec_error_t validate_header_mark(const ir_pulse_train_t *train)
{
    if (!in_window(
            train->durations_us[0],
            NEC_HEADER_MARK_MIN_US,
            NEC_HEADER_MARK_MAX_US)) {
        return NEC_ERROR_HEADER_MARK_OUT_OF_TOLERANCE;
    }
    return NEC_OK;
}

static nec_error_t validate_normal_header_space(
    const ir_pulse_train_t *train)
{
    if (!in_window(
            train->durations_us[1],
            NEC_HEADER_SPACE_MIN_US,
            NEC_HEADER_SPACE_MAX_US)) {
        return NEC_ERROR_HEADER_SPACE_OUT_OF_TOLERANCE;
    }
    return NEC_OK;
}

nec_error_t nec_decode(
    const ir_pulse_train_t *train,
    nec_frame_t *frame)
{
    nec_error_t error;
    uint32_t address = 0;
    uint32_t address_inverse = 0;
    uint32_t command = 0;
    uint32_t command_inverse = 0;
    size_t bit;
    size_t index;

    if (train == NULL || frame == NULL || train->durations_us == NULL) {
        return NEC_ERROR_INVALID_ARGUMENT;
    }
    if (train->length != NEC_NORMAL_PULSE_COUNT &&
        train->length != NEC_REPEAT_PULSE_COUNT) {
        return NEC_ERROR_WRONG_PULSE_COUNT;
    }
    error = validate_header_mark(train);
    if (error != NEC_OK) {
        return error;
    }
    if (train->length == NEC_REPEAT_PULSE_COUNT) {
        if (!in_window(
                train->durations_us[1],
                NEC_REPEAT_SPACE_MIN_US,
                NEC_REPEAT_SPACE_MAX_US)) {
            return NEC_ERROR_REPEAT_SPACE_OUT_OF_TOLERANCE;
        }
        if (!in_window(
                train->durations_us[2],
                NEC_BIT_MARK_MIN_US,
                NEC_BIT_MARK_MAX_US)) {
            return NEC_ERROR_TERMINATING_MARK_OUT_OF_TOLERANCE;
        }
        frame->type = NEC_FRAME_REPEAT;
        frame->address = 0;
        frame->command = 0;
        return NEC_OK;
    }
    error = validate_normal_header_space(train);
    if (error != NEC_OK) {
        return error;
    }

    for (bit = 0; bit < 32; bit++) {
        uint32_t mark = train->durations_us[2 + bit * 2];
        uint32_t space = train->durations_us[3 + bit * 2];
        uint32_t *target;

        if (!in_window(mark, NEC_BIT_MARK_MIN_US, NEC_BIT_MARK_MAX_US)) {
            return NEC_ERROR_BIT_MARK_OUT_OF_TOLERANCE;
        }
        if (in_window(space, NEC_ZERO_SPACE_MIN_US, NEC_ZERO_SPACE_MAX_US)) {
            target = bit < 8 ? &address :
                bit < 16 ? &address_inverse :
                bit < 24 ? &command : &command_inverse;
        } else if (in_window(space, NEC_ONE_SPACE_MIN_US, NEC_ONE_SPACE_MAX_US)) {
            target = bit < 8 ? &address :
                bit < 16 ? &address_inverse :
                bit < 24 ? &command : &command_inverse;
            *target |= (uint32_t)1u << (bit % 8);
        } else {
            return NEC_ERROR_AMBIGUOUS_BIT;
        }
    }
    index = 2 + 32 * 2;
    if (!in_window(
            train->durations_us[index],
            NEC_BIT_MARK_MIN_US,
            NEC_BIT_MARK_MAX_US)) {
        return NEC_ERROR_TERMINATING_MARK_OUT_OF_TOLERANCE;
    }
    if (((uint8_t)address ^ (uint8_t)address_inverse) != 0xFFu ||
        ((uint8_t)command ^ (uint8_t)command_inverse) != 0xFFu) {
        return NEC_ERROR_COMPLEMENT_MISMATCH;
    }
    frame->type = NEC_FRAME_NORMAL;
    frame->address = (uint8_t)address;
    frame->command = (uint8_t)command;
    return NEC_OK;
}

nec_error_t nec_encode(
    uint8_t address,
    uint8_t command,
    ir_pulse_train_t *train)
{
    uint8_t bytes[4] = {address, (uint8_t)~address, command, (uint8_t)~command};
    size_t bit;
    size_t index = 0;

    if (train == NULL || train->durations_us == NULL) {
        return NEC_ERROR_INVALID_ARGUMENT;
    }
    if (train->capacity < NEC_NORMAL_PULSE_COUNT) {
        return NEC_ERROR_OUTPUT_CAPACITY;
    }
    train->durations_us[index++] = NEC_HEADER_MARK_US;
    train->durations_us[index++] = NEC_HEADER_SPACE_US;
    for (size_t byte_index = 0; byte_index < 4; byte_index++) {
        for (bit = 0; bit < 8; bit++) {
            train->durations_us[index++] = NEC_BIT_MARK_US;
            train->durations_us[index++] =
                (bytes[byte_index] & (uint8_t)(1u << bit)) != 0u
                    ? NEC_ONE_SPACE_US : NEC_ZERO_SPACE_US;
        }
    }
    train->durations_us[index++] = NEC_BIT_MARK_US;
    train->length = index;
    return NEC_OK;
}

const char *nec_error_string(nec_error_t error)
{
    switch (error) {
    case NEC_OK:
        return "ok";
    case NEC_ERROR_INVALID_ARGUMENT:
        return "invalid argument";
    case NEC_ERROR_WRONG_PULSE_COUNT:
        return "wrong pulse count";
    case NEC_ERROR_HEADER_MARK_OUT_OF_TOLERANCE:
        return "header mark out of tolerance";
    case NEC_ERROR_HEADER_SPACE_OUT_OF_TOLERANCE:
        return "header space out of tolerance";
    case NEC_ERROR_REPEAT_SPACE_OUT_OF_TOLERANCE:
        return "repeat space out of tolerance";
    case NEC_ERROR_BIT_MARK_OUT_OF_TOLERANCE:
        return "bit mark out of tolerance";
    case NEC_ERROR_AMBIGUOUS_BIT:
        return "ambiguous bit timing";
    case NEC_ERROR_COMPLEMENT_MISMATCH:
        return "complement mismatch";
    case NEC_ERROR_TERMINATING_MARK_OUT_OF_TOLERANCE:
        return "terminating mark out of tolerance";
    case NEC_ERROR_OUTPUT_CAPACITY:
        return "output capacity too small";
    }
    return "unknown error";
}
