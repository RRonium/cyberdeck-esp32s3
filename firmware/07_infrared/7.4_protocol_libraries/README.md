# 7.4 Protocol libraries

This module currently implements NEC encode/decode only. RC5, RC6, and Sony
SIRC are reserved as future protocol slots and are intentionally deferred.

The decoder consumes pulse-duration arrays supplied by
`7.2_pulse_learning/`. It has no knowledge of GPIO, timers, or the 38 kHz
carrier. Carrier demodulation is assumed to have already been performed by an
IR receiver module.

NEC decoding accepts approximately +/-25% timing variation through named
duration windows. It validates the header, each bit mark, zero/one space
classification, terminating mark, and inverted address/command bytes.
Repeat frames are returned as `NEC_FRAME_REPEAT`, not treated as errors.
