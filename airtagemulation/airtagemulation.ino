#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

BLEScan* pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    // Print the advertisement info for each discovered device
    Serial.println("------------------------------------------------");
    Serial.print("Device Name: ");
    Serial.println(advertisedDevice.getName().c_str());
    Serial.print("Address: ");
    Serial.println(advertisedDevice.getAddress().toString().c_str());
    Serial.print("Manufacturer Data: ");
    Serial.println(advertisedDevice.getManufacturerData().c_str());
    Serial.print("RSSI: ");
    Serial.println(advertisedDevice.getRSSI());

    // Optional: Print raw advertisement data
    // Serial.print("Raw Advertisement Data: ");
    // for (int i = 0; i < advertisedDevice.getPayload().length(); i++) {
    //   Serial.print(advertisedDevice.getPayload()[i], HEX);
    //   Serial.print(" ");
    // }
    // Serial.println();
  }
};

void setup() {
  // Start serial communication
  Serial.begin(115200);
  Serial.println("Starting BLE Sniffer...");

  // Initialize BLE
  BLEDevice::init("");

  // Create the BLE scanner
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setActiveScan(true); // Active scan will request more information from devices
  pBLEScan->setInterval(100);    // Scan interval (ms)
  pBLEScan->setWindow(99);       // Scan window (ms)

  // Set the callback function to handle the device scan results
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());

  // Start scanning for 5 seconds
  pBLEScan->start(5, false);
}

void loop() {
  // Keep scanning for 5 seconds, repeat
  Serial.println("Scanning for 5 seconds...");
  pBLEScan->start(5, false);   // Start scanning
  delay(5000);                  // Wait for the scan to finish before starting again
}
