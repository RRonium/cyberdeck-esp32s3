/**
 * @file 4.4_protocol_analysis.ino
 * @brief Frame Header & Checksum Dissector
 * @directory firmware/04_2.4ghz_nrf24/4.4_protocol_analysis/
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

#define NRF_CE_PIN    4
#define NRF_CSN_PIN   5
#define NRF_SCK_PIN   18
#define NRF_MISO_PIN  19
#define NRF_MOSI_PIN  23

SPIClass nrfSPI(FSPI);
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

const uint8_t ANALYZE_ADDR[5] = {0xE7, 0xE7, 0xE7, 0xE7, 0xE7};

// Helper: Calculate simple CRC-CCITT (0x1021) for validation
uint16_t calculateCrc16(const uint8_t *data, size_t len) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < len; i++) {
    crc ^= (uint16_t)data[i] << 8;
    for (uint8_t bit = 0; bit < 8; bit++) {
      if (crc & 0x8000) crc = (crc << 1) ^ 0x1021;
      else crc <<= 1;
    }
  }
  return crc;
}

void setup() {
  Serial.begin(115200);
  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  radio.begin(&nrfSPI);
  radio.setAutoAck(false);
  radio.setPayloadSize(32);
  radio.setChannel(76);
  radio.openReadingPipe(1, ANALYZE_ADDR);
  radio.startListening();

  Serial.println(F("[*] Protocol Dissector Active on Ch 76."));
}

void loop() {
  if (radio.available()) {
    uint8_t payload[32];
    radio.read(&payload, sizeof(payload));

    uint8_t header = payload[0];
    uint8_t seqNum = (header >> 1) & 0x03;
    uint8_t ackFlag = header & 0x01;
    uint16_t payloadCrc = (payload[30] << 8) | payload[31];
    uint16_t calculated = calculateCrc16(payload, 30);

    Serial.printf("PKT RECV | Seq: %u | AckReq: %u | CRC: 0x%04X (%s)\n",
                  seqNum, ackFlag, payloadCrc,
                  (payloadCrc == calculated) ? "VALID" : "UNVERIFIED");
  }
}