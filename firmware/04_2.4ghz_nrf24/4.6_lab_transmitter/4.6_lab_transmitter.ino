/**
 * @file 4.6_lab_transmitter.ino
 * @brief Controlled Reference Signal Transmitter for Test & Benchmarking
 * @directory firmware/04_2.4ghz_nrf24/4.6_lab_transmitter/
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

const uint8_t TX_ADDR[5] = {0x01, 0xAB, 0xCD, 0xEF, 0x01};
uint32_t packetSequence = 0;

void setup() {
  Serial.begin(115200);
  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  if (!radio.begin(&nrfSPI)) {
    Serial.println(F("[!] ERROR: nRF24L01+ initialization failed."));
    while (1) delay(1000);
  }

  radio.setPALevel(RF24_PA_LOW); // Safe benchmark output power (-18 dBm)
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(40);
  radio.openWritingPipe(TX_ADDR);
  radio.stopListening();

  Serial.println(F("[*] Reference Lab Transmitter Active on Ch 40."));
}

void loop() {
  struct __attribute__((packed)) LabPacket {
    uint32_t counter;
    uint32_t uptime;
    char signature[8];
    uint8_t payload[16];
  } testPacket;

  testPacket.counter = packetSequence++;
  testPacket.uptime = millis();
  memcpy(testPacket.signature, "CYBRDCK", 7);
  testPacket.signature[7] = '\0';
  memset(testPacket.payload, 0xAA, sizeof(testPacket.payload));

  bool delivered = radio.write(&testPacket, sizeof(testPacket));

  Serial.printf("[TX] Packet #%lu %s | Uptime: %lu ms\n",
                testPacket.counter, delivered ? "Sent" : "Failed", testPacket.uptime);

  delay(500); // Emit test frame every 500 ms
}