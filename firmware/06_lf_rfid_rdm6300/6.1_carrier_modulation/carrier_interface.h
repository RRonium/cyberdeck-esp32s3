#ifndef RFID_CARRIER_INTERFACE_H
#define RFID_CARRIER_INTERFACE_H

#include <stddef.h>
#include <stdint.h>

/*
 * Future raw-demodulation front-end contract. This interface intentionally
 * has no hardware implementation or assumptions about the physical carrier.
 */
typedef enum {
    RFID_CARRIER_OK = 0,
    RFID_CARRIER_NEED_MORE,
    RFID_CARRIER_INVALID_ARGUMENT
} rfid_carrier_result_t;

typedef struct {
    uint8_t *bits;
    size_t capacity;
    size_t length;
} rfid_raw_bit_buffer_t;

rfid_carrier_result_t rfid_carrier_read_bit(uint8_t *bit);

rfid_carrier_result_t rfid_carrier_read_bits(
    rfid_raw_bit_buffer_t *buffer);

#endif
