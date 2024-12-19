#include <SPI.h>
#include <mcp_can.h>

// CAN Bus settings
const int CAN_CS_PIN = 5; // Chip Select Pin for MCP2515
const int CAN_INT_PIN = 4; // Interrupt Pin
MCP_CAN CAN(CAN_CS_PIN); // Create MCP_CAN instance

void setup() {
  Serial.begin(115200);

  // Initialize CAN Bus at 500kbps
  if (CAN.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("CAN Bus Initialized");
  } else {
    Serial.println("CAN Bus initialization failed");
    while (1); // Stop execution here if CAN initialization fails
  }

  // Set CAN mode to normal
  CAN.setMode(MCP_NORMAL);
  Serial.println("CAN Bus is ready to send messages.");
}

void loop() {
  unsigned long msgId = 0x100; // CAN ID for the message
  byte msg[8] = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88}; // Example data

  // Send the CAN message
  if (CAN.sendMsgBuf(msgId, 0, sizeof(msg), msg) == CAN_OK) {
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Failed to Send Message");
  }

  delay(1000); // Send a message every second
}
