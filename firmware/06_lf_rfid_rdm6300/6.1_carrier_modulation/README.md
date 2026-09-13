# 6.1 Carrier modulation

This module is hardware-dependent and intentionally deferred. The repository
does not yet specify GPIOs, UART pins, baud rate, voltage levels, coil design,
or the exact RDM6300 module variant, so no hardware implementation is assumed.

The interface-only header in this directory is forward-looking scaffolding for
a future raw-signal capture path. It is not part of the current
`6.2_tag_read` to `6.3_rdm6300_payload_decode` data path.
