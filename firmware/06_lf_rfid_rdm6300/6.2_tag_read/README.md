# 6.2 Tag read

This module parses the RDM6300 UART frame at the documented 9600-baud
interface. A complete frame is:

```text
STX (0x02) + 10 ASCII hex payload characters + 2 ASCII hex checksum characters + ETX (0x03)
```

The parser accepts fragmented input one byte at a time. It reports
`RDM6300_PARSE_NEED_MORE` for incomplete input and only reports a typed error
after a complete frame is available. Bytes received while waiting for `STX`
are treated as transport noise. Direct parsing of a complete frame reports a
bad start sentinel when applicable. The checksum is the XOR of the five
decoded payload bytes, not the ASCII characters.

Known limitation: the streaming parser resets fully after a failed 14-byte
capture window and waits for the next fresh `0x02`; it does not rescan the
discarded buffer for an `STX` buried inside it.

The repository does not yet specify the physical UART pins, voltage levels, or
exact RDM6300 module variant. Those remain integration decisions.
