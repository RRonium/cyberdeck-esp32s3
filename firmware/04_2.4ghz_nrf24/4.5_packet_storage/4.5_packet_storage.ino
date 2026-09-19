/**
 * @file 4.5_packet_storage.ino
 * @brief MicroSD Storage Logger for Received 2.4 GHz Packets
 * @directory firmware/04_2.4ghz_nrf24/4.5_packet_storage/
 */

#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <RF24.h>

#define NRF_CE_PIN    4
#define NRF_CSN_PIN   5
#define NRF_SCK_PIN   18
#define NRF_MISO_PIN  19
#define NRF_MOSI_PIN  23
#define SD_CS_PIN     21

SPIClass nrfSPI(FSPI);
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);
File dumpFile;

const uint8_t LOG_ADDR[5] = {0x01, 0xAB, 0xCD, 0xEF, 0x01};

void setup() {
  Serial.begin(115200);
  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  if (!SD.begin(SD_CS_PIN)) {
    Serial.println(F("[!] MicroSD Mount Failed. Check CS pin & card formatting."));
  } else {
    Serial.println(F("[*] MicroSD initialized. Opening log file."));
    dumpFile = SD.open("/rf24_dumps.bin", FILE_APPEND);
  }

  radio.begin(&nrfSPI);
  radio.setChannel(40);
  radio.openReadingPipe(1, LOG_ADDR);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    uint8_t buffer[32];
    radio.read(&buffer, sizeof(buffer));

    if (dumpFile) {
      uint32_t timestamp = millis();
      dumpFile.write((uint8_t*)&timestamp, sizeof(timestamp));
      dumpFile.write(buffer, sizeof(buffer));
      dumpFile.flush();
      Serial.printf("[*] Flushed 32-byte packet at %lu ms to SD.\n", timestamp);
    }
  }
}