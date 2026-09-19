#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>

#define PN532_SCK   (12)
#define PN532_MISO  (13)
#define PN532_MOSI  (11)
#define PN532_SS    (10)

Adafruit_PN532 nfc(PN532_SCK, PN532_MISO, PN532_MOSI, PN532_SS);

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000); 

    Serial.println("[NFC] Initializing PN532...");

    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[NFC-ERR] PN532 board not detected. Check SPI wiring.");
        while (1) { delay(500); }
    }

    Serial.printf("[NFC-OK] Chip found: PN5%02X | Firmware rev: %d.%d\n",
                  (versiondata >> 24) & 0xFF,
                  (versiondata >> 16) & 0xFF,
                  (versiondata >> 8) & 0xFF);

    nfc.SAMConfig();
    Serial.println("[NFC] Waiting for an ISO14443A card...");
}

void loop() {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

    if (success) {
        Serial.println("\n------------------------------------");
        Serial.println("[NFC] Card Detected!");
        Serial.printf("  UID Length : %d bytes\n", uidLength);
        Serial.print("  UID Value  : ");
        for (uint8_t i = 0; i < uidLength; i++) {
            Serial.printf("%02X ", uid[i]);
        }
        Serial.println();
        delay(1500); 
    }
}