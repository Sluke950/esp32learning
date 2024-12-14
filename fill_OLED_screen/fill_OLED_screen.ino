#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin (use -1 if not connected)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  // Initialize the display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);  // Infinite loop if initialization fails
  }

  display.display(); // Actually update the display with the buffer content
  delay(2000); // Wait for 2 seconds to let the screen initialize

  display.clearDisplay();  // Clear the display to start fresh
}

void loop() {
  // Fill the screen column by column
  for (int col = 0; col < SCREEN_WIDTH; col++) {
    // Fill the column from bottom to top
    for (int row = SCREEN_HEIGHT - 1; row >= 0; row--) {
      display.drawPixel(col, row, SSD1306_WHITE);  // Draw pixel in current column and row
      display.display();  // Update the display with the new pixels
      //delay(10);  // Delay to control the speed of filling
    }
  }

  // Remove the pixels in the same pattern (bottom to top, column by column)
  for (int col = 0; col < SCREEN_WIDTH; col++) {
    // Erase the column from bottom to top
    for (int row = SCREEN_HEIGHT - 1; row >= 0; row--) {
      display.drawPixel(col, row, SSD1306_BLACK);  // Erase pixel by drawing it black
      display.display();  // Update the display with the removed pixels
      //delay(10);  // Delay to control the speed of removal
    }
  }

  //delay(500);  // Wait half a second before starting the animation again
}
