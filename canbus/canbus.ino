#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <mcp_can.h>

// OLED Display settings
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// CAN Bus settings
const int CAN_CS_PIN = 5; // Chip Select Pin for MCP2515
const int CAN_INT_PIN = 4; // Interrupt Pin
MCP_CAN CAN(CAN_CS_PIN); // Create MCP_CAN instance

void showSplashScreen() {
  display.clearDisplay();

  // Display "revibe" text for the splash screen
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(20, 20); // Center the text (adjust as needed)
  display.print("revibe");
  display.display();

  delay(3000); // Show splash screen for 3 seconds
}

void setup() {
  Serial.begin(115200);
  
  // Start with display check
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED display failed to initialize");
    while (true); // Stop execution here if display fails
  }
  Serial.println("OLED Display initialized successfully");
  
  // Initialize CAN Bus
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("CAN Bus initialized");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("CAN Bus Initialized");
    display.display();
  } else {
    Serial.println("CAN Bus initialization failed");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("CAN Bus Init Failed");
    display.display();
    while (1); // Stop execution here if CAN fails
  }
  CAN.setMode(MCP_NORMAL); // Set MCP2515 to Normal mode

  // Show splash screen
  showSplashScreen();

  // Print initial message on OLED after splash screen
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print("Waiting for CAN data...");
  display.display();
}

void loop() {
  long unsigned int canId;
  unsigned char len = 0;
  unsigned char buf[8];

  // Check for incoming CAN messages
  if (CAN.checkReceive() == CAN_MSGAVAIL) {
    CAN.readMsgBuf(&canId, &len, buf); // Read CAN message

    // Print CAN message details to Serial Monitor
    Serial.print("CAN ID: ");
    Serial.println(canId, HEX);
    Serial.print("Message Length: ");
    Serial.println(len);
    Serial.print("Data: ");
    for (int i = 0; i < len; i++) {
      Serial.print(buf[i], HEX);
      Serial.print(" ");
    }
    Serial.println();

    // Print CAN message details to OLED
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("CAN ID: ");
    display.println(canId, HEX);
    display.print("Len: ");
    display.println(len);
    display.print("Data: ");
    for (int i = 0; i < len; i++) {
      display.print(buf[i], HEX);
      display.print(" ");
    }
    display.display();
  } else {
    // No CAN messages available
    Serial.println("No CAN message available");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.print("No CAN message available");
    display.display();
  }

  delay(100); // Small delay for stability
}
