# 7.2 Pulse learning

This module defines and validates a pulse train as alternating mark/space
durations in microseconds, beginning with a mark. A capture timestamp is
represented by `epoch_or_0`; it remains zero until an RTC, GPS, or other clock
source is selected.

Validation rejects empty or single-entry captures, durations below 100
microseconds, durations above 15 milliseconds, and captures that exceed their
provided capacity. Callers may also request strict even-length mark/space
pairs, which rejects an unpaired final duration.

NEC frames intentionally contain an odd number of entries because the final
entry is a terminating mark. Therefore NEC validation does not request strict
complete pairs. This layer checks physical plausibility only; protocol timing
classification belongs in `7.4_protocol_libraries/`.
