#include "ir_pulse_train.h"

ir_pulse_error_t ir_pulse_train_validate(
    const ir_pulse_train_t *train,
    uint8_t require_complete_pairs)
{
    size_t index;

    if (train == NULL || train->durations_us == NULL) {
        return IR_PULSE_ERROR_INVALID_ARGUMENT;
    }
    if (train->length == 0) {
        return IR_PULSE_ERROR_EMPTY;
    }
    if (train->length == 1) {
        return IR_PULSE_ERROR_TOO_SHORT;
    }
    if (train->length > train->capacity) {
        return IR_PULSE_ERROR_CAPACITY;
    }
    if (require_complete_pairs != 0 && (train->length % 2u) != 0u) {
        return IR_PULSE_ERROR_UNPAIRED_DURATION;
    }
    for (index = 0; index < train->length; index++) {
        if (train->durations_us[index] < IR_PULSE_MIN_US) {
            return IR_PULSE_ERROR_TOO_SHORT_DURATION;
        }
        if (train->durations_us[index] > IR_PULSE_MAX_US) {
            return IR_PULSE_ERROR_TOO_LONG_DURATION;
        }
    }
    return IR_PULSE_OK;
}

const char *ir_pulse_error_string(ir_pulse_error_t error)
{
    switch (error) {
    case IR_PULSE_OK:
        return "ok";
    case IR_PULSE_ERROR_INVALID_ARGUMENT:
        return "invalid argument";
    case IR_PULSE_ERROR_EMPTY:
        return "empty pulse train";
    case IR_PULSE_ERROR_TOO_SHORT:
        return "pulse train has one entry";
    case IR_PULSE_ERROR_TOO_SHORT_DURATION:
        return "pulse duration is below minimum";
    case IR_PULSE_ERROR_TOO_LONG_DURATION:
        return "pulse duration is above maximum";
    case IR_PULSE_ERROR_UNPAIRED_DURATION:
        return "pulse train has an unpaired duration";
    case IR_PULSE_ERROR_CAPACITY:
        return "pulse train exceeds capacity";
    }
    return "unknown error";
}
