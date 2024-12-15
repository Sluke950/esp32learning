



#include <SPI.h>
#include <SD_MMC.h>

// Define the chip select pin for the SD card module
#define SD_CS_PIN 5

// Define the LED pin
#define LED_PIN 2

File myFile;

void setup() {
  // Start the serial communication
  Serial.begin(115200);

  // Initialize the LED pin
  pinMode(LED_PIN, OUTPUT);

  // Initialize the SD card
  if (!SD_MMC.begin()) {
    Serial.println("SD card initialization failed!");
    // Flash LED to indicate failure
    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
  }
  Serial.println("SD card initialized.");

  // Open a file for writing
  myFile = SD_MMC.open("test.txt", FILE_WRITE);

  if (myFile) {
    Serial.println("Writing to test.txt...");
    myFile.println("Hello, this is a test file written by ESP32.");
    myFile.println("ESP32 and SD card modules are working together!");
    myFile.close();
    Serial.println("Done writing to file.");

    // Turn LED solid to indicate success
    digitalWrite(LED_PIN, HIGH);
  } else {
    Serial.println("Failed to open file for writing.");
    // Flash LED to indicate failure
    while (true) {
      digitalWrite(LED_PIN, HIGH);
      delay(500);
      digitalWrite(LED_PIN, LOW);
      delay(500);
    }
  }
}

void loop() {
  // Do nothing in the loop
}
