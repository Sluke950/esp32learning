#include <SPI.h>
#include <SD.h>

// Define SPI pins
#define SD_CS 5   // Chip Select pin
#define SD_SCK 18 // SPI Clock
#define SD_MISO 19 // Master In, Slave Out
#define SD_MOSI 23 // Master Out, Slave In

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing SD card...");

  // Initialize SPI with custom pins
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    Serial.println("SD Card initialization failed!");
    while (1); // Halt program if SD initialization fails
  }

  Serial.println("SD Card initialized successfully!");
}

void loop() {
  // Your code here
}
