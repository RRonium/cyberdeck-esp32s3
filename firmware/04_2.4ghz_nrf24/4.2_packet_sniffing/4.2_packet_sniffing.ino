/**
 * @file 4.2_packet_sniffing.ino
 * @brief Promiscuous 2.4 GHz Frame Capture Routine
 * @directory firmware/04_2.4ghz_nrf24/4.2_packet_sniffing/
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

const uint8_t TARGET_CHANNEL = 40; // 2440 MHz
const uint8_t PROMISCUOUS_ADDR[5] = {0xAA, 0x55, 0xAA, 0x55, 0xAA};

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  if (!radio.begin(&nrfSPI)) {
    Serial.println(F("[!] ERROR: nRF24L01+ initialization failed."));
    while (1) delay(1000);
  }

  // Raw reception configuration
  radio.setAutoAck(false);
  radio.disableCRC();
  radio.setAddressWidth(3); // Minimum address filter size
  radio.setDataRate(RF24_1MBPS);
  radio.setChannel(TARGET_CHANNEL);

  radio.openReadingPipe(1, PROMISCUOUS_ADDR);
  radio.startListening();

  Serial.printf("[*] Raw sniffing initialized on Ch: %d (24%02d MHz)\n", TARGET_CHANNEL, TARGET_CHANNEL);
}

void loop() {
  if (radio.available()) {
    uint8_t buffer[32];
    radio.read(&buffer, sizeof(buffer));

    Serial.printf("[%lu ms] LEN: %d | HEX: ", millis(), sizeof(buffer));
    for (uint8_t i = 0; i < sizeof(buffer); i++) {
      Serial.printf("%02X ", buffer[i]);
    }
    Serial.println();
  }
}