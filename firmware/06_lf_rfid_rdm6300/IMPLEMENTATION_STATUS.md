# LF RFID RDM6300 Implementation Status

## Completed work

### Repository structure

- Renamed `6.3_em4100_decoding/` to `6.3_rdm6300_payload_decode/`.
- Updated the repository structure shown in the root `README.md`.
- Preserved the existing numbered module layout.
- Added a root `.gitignore` for `.DS_Store` files and host-side C build artifacts.

### `6.2_tag_read`

Implemented a host-compilable RDM6300 UART frame parser:

```text
STX (0x02) + 10 ASCII hex payload characters
+ 2 ASCII hex checksum characters + ETX (0x03)
```

The parser:

- Requires a complete 14-byte frame for direct parsing.
- Converts the ten payload hex characters into five decoded bytes.
- Converts the two checksum characters into one checksum byte.
- Validates the `STX` and `ETX` sentinels.
- Validates every hexadecimal character.
- Computes the checksum as XOR over the five decoded payload bytes.
- Returns explicit typed errors for malformed complete frames.
- Supports fragmented byte-by-byte input.
- Ignores transport noise received while waiting for `STX`.

Files:

- `6.2_tag_read/rdm6300_frame_parser.h`
- `6.2_tag_read/rdm6300_frame_parser.c`
- `6.2_tag_read/README.md`

### `6.3_rdm6300_payload_decode`

Implemented decoding of the validated five-byte RDM6300 payload:

- Extracts the version/customer byte.
- Converts the four-byte tag identifier to `uint32_t`.
- Produces an uppercase eight-character hexadecimal identifier.
- Does not perform EM4100 parity validation because the RDM6300 UART
  payload does not contain the raw EM4100 parity bitstream.

Files:

- `6.3_rdm6300_payload_decode/rdm6300_payload_decoder.h`
- `6.3_rdm6300_payload_decode/rdm6300_payload_decoder.c`
- `6.3_rdm6300_payload_decode/README.md`

### `6.4_signature_storage`

Implemented an in-memory signature record containing:

- Complete raw RDM6300 frame bytes.
- Decoded 32-bit unique identifier.
- Protocol enum with an RDM6300/EM4100 value.
- Checksum-validation status.
- `epoch_or_0` timestamp field.

The timestamp remains zero until an RTC, GPS, or other clock source is
selected. Records are intended for the existing
`data_dumps/rfid_dumps/` location.

Files:

- `6.4_signature_storage/rdm6300_signature_record.h`
- `6.4_signature_storage/rdm6300_signature_record.c`
- `6.4_signature_storage/README.md`

## Integration notes

- The cross-directory relative includes from the 6.4 headers into 6.2 and
  6.3 are intentional for now, but are a known build-system fragility. Keep
  the coupling visible when a future build system is introduced.
- `checksum_valid` is always `1` by construction until a failed-capture
  logging path exists; it is not currently a live check.

## Offline validation

`6.2_tag_read/test_rdm6300.c` covers:

- Valid frame parsing.
- Decoded-byte XOR checksum validation.
- Payload decoding and hexadecimal formatting.
- Signature-record initialization.
- Fragmented input assembled across multiple reads.
- Noise before a frame.
- Wrong frame length.
- Invalid hexadecimal characters.
- Invalid start sentinel.
- Invalid end sentinel.
- Checksum mismatch.

The implementation compiles as plain C on the host with strict warnings:

```text
cc -std=c11 -Wall -Wextra -Werror -pedantic
```

No ESP-IDF, Arduino, build system, GPIO access, or hardware-specific
dependency has been introduced.

## Deferred hardware work

The following remain intentionally unimplemented because the repository does
not yet specify the exact hardware details:

- `6.1_carrier_modulation/`
- `6.5_t5577_write/`
- `6.6_coil_emulation/`
- Physical UART pin assignments.
- UART voltage levels and electrical protection.
- Exact RDM6300 module variant.
- LF antenna and coil design.
- Carrier generation and modulation circuitry.
- T5577 write timing and hardware interface.
- Persistent file serialization and MicroSD integration.
- Timestamp acquisition from an RTC, GPS, or other clock source.
