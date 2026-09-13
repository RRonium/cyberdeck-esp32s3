#ifndef IR_PULSE_TRAIN_H
#define IR_PULSE_TRAIN_H

#include <stddef.h>
#include <stdint.h>

#define IR_PULSE_MIN_US 100u
#define IR_PULSE_MAX_US 15000u

typedef enum {
    IR_PULSE_OK = 0,
    IR_PULSE_ERROR_INVALID_ARGUMENT,
    IR_PULSE_ERROR_EMPTY,
    IR_PULSE_ERROR_TOO_SHORT,
    IR_PULSE_ERROR_TOO_SHORT_DURATION,
    IR_PULSE_ERROR_TOO_LONG_DURATION,
    IR_PULSE_ERROR_UNPAIRED_DURATION,
    IR_PULSE_ERROR_CAPACITY
} ir_pulse_error_t;

typedef struct {
    uint32_t *durations_us;
    size_t length;
    size_t capacity;
    uint32_t epoch_or_0;
} ir_pulse_train_t;

ir_pulse_error_t ir_pulse_train_validate(
    const ir_pulse_train_t *train,
    uint8_t require_complete_pairs);

const char *ir_pulse_error_string(ir_pulse_error_t error);

#endif
