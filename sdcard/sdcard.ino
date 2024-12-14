#include <SPI.h>
#include <SD_MMC.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void displayMessage(const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(message);
  display.display();
}

void setup() {
  // Start serial communication
  Serial.begin(115200);

  // Initialize the OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    displayMessage("OLED Error");
    for (;;);  // Halt further execution if OLED fails
  }
  displayMessage("Initializing...");

  // Initialize SD_MMC
  if (!SD_MMC.begin()) {
    Serial.println("SD card initialization failed!");
    displayMessage("SD init failed");
    return;
  }
  Serial.println("SD card initialized.");
  displayMessage("SD Card Init OK");

  // Create or open the file
  File testFile = SD_MMC.open("/test.txt", FILE_WRITE);

  if (testFile) {
    // Write some text to the file
    testFile.println("This is a test message.");
    testFile.println("SD card is working!");
    testFile.close();  // Don't forget to close the file
    Serial.println("File created and text written.");
    displayMessage("File written OK");
  } else {
    // Error opening the file
    Serial.println("Error opening the file.");
    displayMessage("File open error");
  }
}

void loop() {
  // Nothing to do in the loop
}
