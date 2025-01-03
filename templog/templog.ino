#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"
#include "DHT.h"
#include <SPI.h>
#include <SD.h>
#include <WebServer.h>

// OLED display configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET    -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Wi-Fi credentials
const char* ssid = "shuttleworth_reliable";
const char* password = "Password$1";

// NTP server configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -6 * 3600; // Adjust for your timezone
const int daylightOffset_sec = 3600;

// DHT22 configuration
#define DHTPIN 4  // GPIO pin for DHT22
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

// SD Card configuration
#define SD_CS 5  // Chip Select pin for SD card module

File logFile;

// Log interval in milliseconds (e.g., 5000 ms = 5 seconds)
unsigned long logInterval = 1000*60*5; //5 minutes
unsigned long lastLogTime = 0;

// Web server instance
WebServer server(80);

void displayMessage(const char* message) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print(message);
  display.display();
}

void showSplashScreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.print("Welcome!");
  display.display();
  delay(2000); // Show splash screen for 2 seconds
}

void showError(const char* errorMessage) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.print("Error:");
  display.setCursor(0, 10);
  display.print(errorMessage);
  display.display();
}

void handleFileRequest() {
  String path = server.uri();
  Serial.print("Requested path: ");
  Serial.println(path);

  if (path == "/") {
    path = "/index.html";
  }

  if (!SD.exists(path)) {
    Serial.println("File not found on SD card");
    server.send(404, "text/plain", "File Not Found");
    return;
  }

  File file = SD.open(path, FILE_READ);
  if (!file) {
    Serial.println("Failed to open file");
    server.send(500, "text/plain", "Failed to open file");
    return;
  }

  String contentType = "text/plain";
  if (path.endsWith(".html")) contentType = "text/html";
  else if (path.endsWith(".css")) contentType = "text/css";
  else if (path.endsWith(".js")) contentType = "application/javascript";
  else if (path.endsWith(".png")) contentType = "image/png";
  else if (path.endsWith(".jpg")) contentType = "image/jpeg";
  else if (path.endsWith(".ico")) contentType = "image/x-icon";

  server.streamFile(file, contentType);
  file.close();
  Serial.println("File served successfully");
}

// Endpoint for current readings
void handleReadingsRequest() {
  // Get temperature and humidity
  float temperatureC = dht.readTemperature();
  float temperatureF = dht.readTemperature(true);
  float humidity = dht.readHumidity();

  // Check for reading failure
  if (isnan(temperatureC) || isnan(temperatureF) || isnan(humidity)) {
    server.send(500, "text/plain", "Failed to read from DHT sensor");
    return;
  }

  // Format and send response
  String response = "Temperature: " + String(temperatureF) + " F (" + String(temperatureC) + " C)\n";
  response += "Humidity: " + String(humidity) + "%";
  
  server.send(200, "text/plain", response);
}

void setup() {
  Serial.begin(115200);

  // Initialize OLED display
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    const char* error = "SSD1306 allocation failed";
    Serial.println(error);
    showError(error);
    for (;;);
  }

  // Show splash screen
  showSplashScreen();

  // Initialize DHT sensor
  dht.begin();

  // Initialize SD card
  if (!SD.begin(SD_CS)) {
    const char* error = "SD init failed";
    Serial.println(error);
    showError(error);
    for (;;);
  }
  Serial.println("SD card initialized.");

  // Log file setup
  if (!SD.exists("/data_log.csv")) {
    logFile = SD.open("/data_log.csv", FILE_WRITE);
    if (logFile) {
      logFile.println("Date,Time,Temperature_F,Temperature_C,Humidity");
      logFile.close();
    } else {
      const char* error = "File creation failed";
      Serial.println(error);
      showError(error);
      for (;;);
    }
  }

  // Connect to Wi-Fi
  Serial.print("Connecting to Wi-Fi");
  WiFi.begin(ssid, password);
  int retryCount = 0;
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    retryCount++;
    if (retryCount > 20) { // Timeout after 10 seconds
      const char* error = "Wi-Fi connect failed";
      Serial.println(error);
      showError(error);
      return;
    }
  }
  Serial.println("\nWi-Fi connected");
  displayMessage("Wi-Fi connected");
  delay(2000);

  // Configure NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  // Setup web server
  server.onNotFound(handleFileRequest);
  server.on("/test", []() {
    server.send(200, "text/plain", "Web server is working!");
  });

  // New endpoint for current readings
  server.on("/readings", HTTP_GET, handleReadingsRequest);

  server.begin();
  Serial.println("Web server started.");
}

void loop() {
  // Handle web server
  server.handleClient();

  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    const char* error = "Failed to obtain time";
    Serial.println(error);
    showError(error);
    delay(2000);
    return;
  }

  // Get temperature and humidity
  float temperatureC = dht.readTemperature();
  float temperatureF = dht.readTemperature(true);
  float humidity = dht.readHumidity();

  if (isnan(temperatureC) || isnan(temperatureF) || isnan(humidity)) {
    const char* error = "DHT read failed";
    Serial.println(error);
    showError(error);
    delay(2000);
    return;
  }

  // Format time as HH:MM:SS
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  // Format date as YYYY-MM-DD
  char dateStr[11];
  strftime(dateStr, sizeof(dateStr), "%Y-%m-%d", &timeinfo);

  // Display the data on the OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.printf("%s %s", dateStr, timeStr);
  display.setCursor(0, 10);
  display.printf("%.2f F %.2f C", temperatureF, temperatureC);
  display.setCursor(0, 20);
  display.printf("%.2f %%", humidity);
  display.setCursor(0, 40);
  display.printf("IP: %s", WiFi.localIP().toString().c_str());
  display.display();

  // Log data to SD card if logInterval has passed
  unsigned long currentMillis = millis();
  if (currentMillis - lastLogTime >= logInterval) {
    logFile = SD.open("/data_log.csv", FILE_APPEND);
    if (logFile) {
      logFile.printf("%s,%s,%.2f,%.2f,%.2f\n", dateStr, timeStr, temperatureF, temperatureC, humidity);
      logFile.close();
    } else {
      Serial.println("Failed to write to file");
    }
    lastLogTime = currentMillis;  // Update last log time
  }

  // Delay before next reading (not the same as logInterval)
  delay(1000);
}
