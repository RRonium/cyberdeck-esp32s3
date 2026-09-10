# Cyberdeck: ESP32-S3 Multi-Protocol Field Security & Diagnostic Platform

A compact, low-cost (~₹3,330), open-source hardware cyberdeck built on the ESP32-S3 microcontroller. Designed for portable wireless protocol auditing, diagnostic signal analysis, and field packet capture across standard short-range and sub-gigahertz RF spectra.

## Key Hardware Specifications

* **Core Processor:** ESP32-S3 Dual-Core Xtensa LX7 (N16R8)
* **Display & Control:** 2.8" SPI TFT Display (ILI9341) with D-Pad / Rotary Encoder
* **Wireless Subsystems:**
  * **2.4 GHz:** Native Wi-Fi (802.11 b/g/n) + BLE 5.0 + nRF24L01+ Transceiver
  * **Sub-GHz:** CC1101 Transceiver (315 / 433 / 868 / 915 MHz)
  * **RFID / NFC:** PN532 (13.56 MHz High-Frequency) + RDM6300 (125 kHz Low-Frequency)
  * **Optical:** 38 kHz Infrared Receiver & Transmitter Pair
* **Storage & Power:** MicroSD SPI Module (pcap logging) + TP4056 LiPo Charging System

---

## Repository Structure

```text
cyberdeck-esp32s3/
├── README.md
├── docs/
│   ├── schematics.pdf
│   ├── pinout_matrix.md
│   └── presentation_deck.pdf
├── hardware/
│   ├── cad/
│   └── pcb_layout/
├── data_dumps/
│   ├── pcap_captures/
│   └── rfid_dumps/
└── firmware/
    ├── 01_wifi/
    │   ├── 1.1_wifi_scan/
    │   ├── 1.2_wardriving/
    │   ├── 1.3_promiscuous_sniffing/
    │   ├── 1.4_network_discovery/
    │   ├── 1.5_ap_hosting/
    │   └── 1.6_frame_lab_testing/
    ├── 02_ble/
    │   ├── 2.1_ble_scan/
    │   ├── 2.2_ble_services/
    │   ├── 2.3_ble_sniffing/
    │   ├── 2.4_custom_beacons/
    │   ├── 2.5_gatt_exploration/
    │   └── 2.6_ble_device_testing/
    ├── 03_subghz_cc1101/
    │   ├── 3.1_rssi_detection/
    │   ├── 3.2_signal_scan/
    │   ├── 3.3_raw_ook_fsk_capture/
    │   ├── 3.4_signal_replay/
    │   ├── 3.5_protocol_decoding/
    │   └── 3.6_subghz_transmit/
    ├── 04_2.4ghz_nrf24/
    │   ├── 4.1_spectrum_scan/
    │   ├── 4.2_packet_sniffing/
    │   ├── 4.3_address_discovery/
    │   ├── 4.4_protocol_analysis/
    │   ├── 4.5_packet_storage/
    │   └── 4.6_lab_transmitter/
    ├── 05_nfc_pn532/
    │   ├── 5.1_nfc_detection/
    │   ├── 5.2_iso14443_read/
    │   ├── 5.3_memory_structure/
    │   ├── 5.4_ndef_rw/
    │   ├── 5.5_dump_storage/
    │   └── 5.6_card_emulation/
    ├── 06_lf_rfid_rdm6300/
    │   ├── 6.1_carrier_modulation/
    │   ├── 6.2_tag_read/
    │   ├── 6.3_em4100_decoding/
    │   ├── 6.4_signature_storage/
    │   ├── 6.5_t5577_write/
    │   └── 6.6_coil_emulation/
    ├── 07_infrared/
    │   ├── 7.1_38khz_modulation/
    │   ├── 7.2_pulse_learning/
    │   ├── 7.3_signal_replay/
    │   ├── 7.4_protocol_libraries/
    │   ├── 7.5_signal_analysis/
    │   └── 7.6_led_circuit_tests/
    └── 00_integrated_cyberdeck_os/