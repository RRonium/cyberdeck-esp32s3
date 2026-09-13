# 6.4 Signature storage

`rdm6300_signature_record_t` is the in-memory record for a validated tag read.
It retains the complete raw RDM6300 frame, decoded unique ID, protocol enum,
checksum-validation status, and a timestamp field.

`epoch_or_0` is zero until a clock source such as an RTC or GPS is selected.
Records are intended to be written under the existing
`data_dumps/rfid_dumps/` directory. No file format or filesystem dependency is
introduced until storage integration is defined.

`checksum_valid` is always `1` by construction until a failed-capture logging
path exists; do not treat it as a live check yet.
