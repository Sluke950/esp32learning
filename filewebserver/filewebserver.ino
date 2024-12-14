#include <WiFi.h>
#include <SPIFFS.h>
 #include <ESPAsyncWebServer.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi credentials
const char* ssid = "shuttleworth_reliable";
const char* password = "Password$1";

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

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

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    displayMessage("OLED init failed");
    return;
  }
  displayMessage("OLED Initialized");

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");
  displayMessage("Wi-Fi connected");

  // Initialize SPIFFS
  if (!SPIFFS.begin(true)) {
    displayMessage("SPIFFS Mount Failed");
    return;
  }

  // Serve the file list
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String fileList = "<h1>Files in SPIFFS</h1><ul>";
    File root = SPIFFS.open("/");
    if (root) {
      File file = root.openNextFile();
      while (file) {
        fileList += "<li><a href='/download";
        fileList += file.name();
        fileList += "'>";
        fileList += file.name();
        fileList += "</a></li>";
        file = root.openNextFile();
      }
    }
    fileList += "</ul>";
    request->send(200, "text/html", fileList);
  });

  // Handle file download
  server.on("/download/*", HTTP_GET, [](AsyncWebServerRequest *request){
    String filePath = "/"+request->path().substring(10);  // remove "/download/"
    if (SPIFFS.exists(filePath)) {
      File file = SPIFFS.open(filePath, "r");
      request->send(200, "application/octet-stream", file);
    } else {
      request->send(404, "text/plain", "File not found");
    }
  });

  // Start the server
  server.begin();

  // Display a message on the screen
  displayMessage("Web server running");
}

void loop() {
  // Check for incoming serial messages and display them on the screen
  if (Serial.available()) {
    String serialData = Serial.readString();
    displayMessage(serialData.c_str());
    Serial.println(serialData);  // This will also print to the Serial Monitor
  }
}
