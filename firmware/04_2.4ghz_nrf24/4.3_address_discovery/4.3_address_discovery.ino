/**
 * @file 4.3_address_discovery.ino
 * @brief Automated Address & Preamble Discovery Routine
 * @directory firmware/04_2.4ghz_nrf24/4.3_address_discovery/
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

// Standard preamble prefixes often seen in embedded transceivers
const uint8_t PREAMBLES[] = {0x55, 0xAA, 0xA5, 0x5A};

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  if (!radio.begin(&nrfSPI)) {
    Serial.println(F("[!] ERROR: nRF24L01+ initialization failed."));
    while (1) delay(1000);
  }

  radio.setAutoAck(false);
  radio.disableCRC();
  radio.setAddressWidth(3);
  radio.setDataRate(RF24_2MBPS);

  Serial.println(F("[*] Scanner active: Dwell-testing common preamble patterns..."));
}

void loop() {
  for (uint8_t ch = 2; ch <= 84; ch += 2) {
    radio.setChannel(ch);

    for (uint8_t p = 0; p < sizeof(PREAMBLES); p++) {
      uint8_t testAddr[3] = {PREAMBLES[p], 0x55, 0xAA};
      radio.openReadingPipe(1, testAddr);
      radio.startListening();

      unsigned long startDwell = millis();
      while (millis() - startDwell < 10) { // 10ms channel dwell
        if (radio.available()) {
          uint8_t frame[32];
          radio.read(&frame, sizeof(frame));

          Serial.printf("[+] Sync hit on Ch %02d | Preamble 0x%02X | Addr Match: %02X %02X %02X\n",
                        ch, PREAMBLES[p], frame[0], frame[1], frame[2]);
        }
      }
      radio.stopListening();
    }
  }
}