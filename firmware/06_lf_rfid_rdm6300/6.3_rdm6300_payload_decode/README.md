# 6.3 RDM6300 payload decode

This module consumes only the five decoded payload bytes produced by
`6.2_tag_read`: one version/customer byte followed by four bytes of unique
identifier. It returns the first byte, the identifier as a `uint32_t`, and an
uppercase eight-character hexadecimal representation.

No EM4100 row or column parity is checked here. The RDM6300 performs the
demodulation and reports the tag payload plus its transport checksum; raw tag
parity bits are not present in this UART payload.
