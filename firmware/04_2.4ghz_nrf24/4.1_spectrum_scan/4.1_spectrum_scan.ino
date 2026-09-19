/**
 * @file 4.1_spectrum_scan.ino
 * @brief 2.4 GHz ISM Band Spectrum Scanner for ESP32-S3 & nRF24L01+
 * @part_of Cyberdeck ESP32-S3 Field Diagnostic Platform
 *
 * Repository: RRonium/cyberdeck-esp32s3
 * Directory:  firmware/04_2.4ghz_nrf24/4.1_spectrum_scan/
 *
 * Scans channels 0–125 across the 2.4 GHz band and measures relative carrier 
 * activity via the nRF24L01+ Received Power Detector (RPD / CD) register.
 */

#include <Arduino.h>
#include <SPI.h>
#include <RF24.h>

/* ==========================================================================
 * Hardware Pin Mapping (ESP32-S3 FSPI / Custom SPI Assignment)
 * Adjust to match docs/pinout_matrix.md if custom pins are mapped.
 * ========================================================================== */
#ifndef NRF_CE_PIN
#define NRF_CE_PIN    4   // Chip Enable
#endif

#ifndef NRF_CSN_PIN
#define NRF_CSN_PIN   5   // SPI Chip Select Not
#endif

#ifndef NRF_SCK_PIN
#define NRF_SCK_PIN   18  // SPI SCK
#endif

#ifndef NRF_MISO_PIN
#define NRF_MISO_PIN  19  // SPI MISO
#endif

#ifndef NRF_MOSI_PIN
#define NRF_MOSI_PIN  23  // SPI MOSI
#endif

/* ==========================================================================
 * Configuration Constants
 * ========================================================================== */
constexpr uint8_t NUM_CHANNELS      = 126; // 2400 MHz to 2525 MHz
constexpr uint16_t SAMPLES_PER_SCAN = 120; // Dwell sampling iterations per channel
constexpr uint32_t SERIAL_BAUD      = 115200;

// Relative signal density array across channels 0–125
uint8_t channel_hits[NUM_CHANNELS] = {0};

// Hardware interface
SPIClass nrfSPI(FSPI);
RF24 radio(NRF_CE_PIN, NRF_CSN_PIN);

/* ==========================================================================
 * Helper Routines
 * ========================================================================== */

void printHeader() {
  Serial.println(F("\n=============================================================="));
  Serial.println(F("  ESP32-S3 Cyberdeck: 2.4 GHz RF Spectrum Analyzer (nRF24)    "));
  Serial.println(F("=============================================================="));
  Serial.println(F("Ch:  00   10   20   30   40   50   60   70   80   90  100  110  120"));
  Serial.println(F("     |    |    |    |    |    |    |    |    |    |    |    |    |  "));
}

void renderWaterfall() {
  // ASCII intensity scale (blank -> dense activity)
  const char glyphs[] = " .:-=+*#%@";
  const uint8_t glyph_count = sizeof(glyphs) - 1;

  char lineBuffer[NUM_CHANNELS + 1];

  for (uint8_t i = 0; i < NUM_CHANNELS; i++) {
    // Map hits (0 to SAMPLES_PER_SCAN) onto glyph scale
    uint8_t index = map(channel_hits[i], 0, SAMPLES_PER_SCAN, 0, glyph_count - 1);
    lineBuffer[i] = glyphs[index];
  }
  lineBuffer[NUM_CHANNELS] = '\0';

  Serial.println(lineBuffer);
}

/* ==========================================================================
 * Setup & Loop
 * ========================================================================== */

void setup() {
  Serial.begin(SERIAL_BAUD);
  while (!Serial && millis() < 2000);

  Serial.println(F("[*] Initializing SPI bus..."));
  nrfSPI.begin(NRF_SCK_PIN, NRF_MISO_PIN, NRF_MOSI_PIN, NRF_CSN_PIN);

  Serial.println(F("[*] Initializing nRF24L01+ transceiver..."));
  if (!radio.begin(&nrfSPI)) {
    Serial.println(F("[!] ERROR: nRF24L01+ hardware not responding. Check wiring."));
    while (true) {
      delay(1000);
    }
  }

  // Configure nRF24 in continuous receiver mode with CRC disabled for raw detection
  radio.setAutoAck(false);
  radio.disableCRC();
  radio.setDataRate(RF24_1MBPS);
  radio.startListening();

  printHeader();
}

void loop() {
  // Clear hit register for the current sweep
  memset(channel_hits, 0, sizeof(channel_hits));

  // Multi-pass sweep across all 126 channels to capture intermittent bursts
  for (uint16_t sample = 0; sample < SAMPLES_PER_SCAN; sample++) {
    for (uint8_t ch = 0; ch < NUM_CHANNELS; ch++) {
      radio.setChannel(ch);
      radio.startListening();
      delayMicroseconds(130); // Allow PLL to lock and settle
      radio.stopListening();

      // testCarrier() queries the RPD register (bit 0 of register 0x09)
      if (radio.testCarrier()) {
        channel_hits[ch]++;
      }
    }
  }

  // Stream output
  renderWaterfall();

  // Periodic header refresh every ~40 lines
  static uint16_t lineCount = 0;
  if (++lineCount >= 40) {
    lineCount = 0;
    printHeader();
  }
}