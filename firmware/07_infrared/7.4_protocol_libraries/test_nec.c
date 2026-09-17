#include "nec_protocol.h"

#include <assert.h>
#include <string.h>

static void test_pulse_validation(void)
{
    uint32_t durations[] = {9000, 4500, 562};
    ir_pulse_train_t train = {durations, 3, 3, 0};
    uint32_t one[] = {562};
    ir_pulse_train_t single = {one, 1, 1, 0};
    uint32_t short_duration[] = {99, 562};
    ir_pulse_train_t too_short = {short_duration, 2, 2, 0};
    uint32_t long_duration[] = {15001, 562};
    ir_pulse_train_t too_long = {long_duration, 2, 2, 0};

    assert(ir_pulse_train_validate(&train, 0) == IR_PULSE_OK);
    assert(ir_pulse_train_validate(&train, 1) ==
           IR_PULSE_ERROR_UNPAIRED_DURATION);
    assert(ir_pulse_train_validate(&single, 0) == IR_PULSE_ERROR_TOO_SHORT);
    assert(ir_pulse_train_validate(&too_short, 0) ==
           IR_PULSE_ERROR_TOO_SHORT_DURATION);
    assert(ir_pulse_train_validate(&too_long, 0) ==
           IR_PULSE_ERROR_TOO_LONG_DURATION);
}

static void test_nec_round_trip(void)
{
    uint32_t durations[NEC_NORMAL_PULSE_COUNT];
    ir_pulse_train_t train = {
        durations, 0, NEC_NORMAL_PULSE_COUNT, 0
    };
    nec_frame_t frame;

    assert(nec_encode(0x10, 0xA5, &train) == NEC_OK);
    assert(train.length == NEC_NORMAL_PULSE_COUNT);
    assert(nec_decode(&train, &frame) == NEC_OK);
    assert(frame.type == NEC_FRAME_NORMAL);
    assert(frame.address == 0x10);
    assert(frame.command == 0xA5);
}

static void test_repeat_frame(void)
{
    uint32_t durations[] = {9000, 2250, 562};
    ir_pulse_train_t train = {durations, 3, 3, 0};
    nec_frame_t frame;

    assert(nec_decode(&train, &frame) == NEC_OK);
    assert(frame.type == NEC_FRAME_REPEAT);
}

static void test_decode_rejections(void)
{
    uint32_t durations[NEC_NORMAL_PULSE_COUNT];
    ir_pulse_train_t train = {
        durations, 0, NEC_NORMAL_PULSE_COUNT, 0
    };
    nec_frame_t frame;
    uint32_t original;

    assert(nec_encode(0x10, 0xA5, &train) == NEC_OK);

    durations[0] = 6000;
    assert(nec_decode(&train, &frame) ==
           NEC_ERROR_HEADER_MARK_OUT_OF_TOLERANCE);
    assert(nec_encode(0x10, 0xA5, &train) == NEC_OK);

    durations[0] = 12000;
    assert(nec_decode(&train, &frame) ==
           NEC_ERROR_HEADER_MARK_OUT_OF_TOLERANCE);
    assert(nec_encode(0x10, 0xA5, &train) == NEC_OK);

    durations[3] = 1000;
    assert(nec_decode(&train, &frame) == NEC_ERROR_AMBIGUOUS_BIT);
    assert(nec_encode(0x10, 0xA5, &train) == NEC_OK);

    original = durations[3];
    durations[3] = durations[3] == NEC_ZERO_SPACE_US
        ? NEC_ONE_SPACE_US : NEC_ZERO_SPACE_US;
    assert(nec_decode(&train, &frame) == NEC_ERROR_COMPLEMENT_MISMATCH);
    durations[3] = original;

    train.length = NEC_NORMAL_PULSE_COUNT - 1;
    assert(nec_decode(&train, &frame) == NEC_ERROR_WRONG_PULSE_COUNT);
}

int main(void)
{
    test_pulse_validation();
    test_nec_round_trip();
    test_repeat_frame();
    test_decode_rejections();
    return 0;
}
