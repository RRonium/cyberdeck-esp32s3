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

    Serial.println("[NFC-READ] Initializing PN532...");

    nfc.begin();
    uint32_t versiondata = nfc.getFirmwareVersion();
    if (!versiondata) {
        Serial.println("[NFC-ERR] PN532 board not detected.");
        while (1) { delay(500); }
    }

    nfc.SAMConfig();
    Serial.println("[NFC-READ] Waiting for an ISO14443A card to read...");
}

void loop() {
    uint8_t success;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
    uint8_t uidLength;

    success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 1000);

    if (success) {
        Serial.println("\n------------------------------------");
        Serial.println("[NFC-READ] Card Detected!");

        if (uidLength == 4) {
            Serial.println("[NFC-READ] MIFARE Classic detected. Authenticating Block 4...");
            
            // Standard default key A
            uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
            
            success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
            
            if (success) {
                Serial.println("[NFC-READ] Authentication successful.");
                uint8_t data[16];
                
                success = nfc.mifareclassic_ReadDataBlock(4, data);
                
                if (success) {
                    Serial.print("[NFC-READ] Block 4 Hex: ");
                    for (uint8_t i = 0; i < 16; i++) {
                        Serial.printf("%02X ", data[i]);
                    }
                    Serial.println();
                    
                    Serial.print("[NFC-READ] Block 4 ASCII: ");
                    for (uint8_t i = 0; i < 16; i++) {
                        if (data[i] >= 32 && data[i] <= 126) Serial.print((char)data[i]);
                        else Serial.print(".");
                    }
                    Serial.println();
                } else {
                    Serial.println("[NFC-ERR] Failed to read block 4.");
                }
            } else {
                Serial.println("[NFC-ERR] Authentication failed with default Key A.");
            }
        } else {
            Serial.println("[NFC-READ] Non-MIFARE Classic card detected (7-byte UID / Ultralight / NTAG).");
        }
        Serial.println("------------------------------------");
        delay(2000);
    }
}