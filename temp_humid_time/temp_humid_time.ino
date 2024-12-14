#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WiFi.h>
#include "time.h"
#include "DHT.h"

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

// DHT configuration
#define DHTPIN 4 // Pin connected to DHT22 sensor
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

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

  // Clear the display after the splash screen
  display.clearDisplay();
  display.display();

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

  // Initialize DHT sensor
  dht.begin();
}

void loop() {
  // Get current time
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    const char* error = "Failed to obtain time";
    Serial.println(error);
    showError(error);
    delay(2000);
    return;
  }

  // Format time as HH:MM:SS
  char timeStr[10];
  strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);

  // Get temperature and humidity
  float temperatureC = dht.readTemperature();
  float temperatureF = dht.readTemperature(true);
  float humidity = dht.readHumidity();

  if (isnan(temperatureC) || isnan(temperatureF) || isnan(humidity)) {
    const char* error = "Sensor error";
    Serial.println(error);
    showError(error);
    delay(2000);
    return;
  }

  // Display data on OLED
  display.clearDisplay();
  display.setTextSize(2); // Normal text size
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  // Display time
  //display.print("Time: ");
  display.println(timeStr);

  // Display temperature
  display.print(temperatureF, 2);
  display.println(" F ");
  display.print(temperatureC, 2);
  display.println(" C");

  // Display humidity
  display.print(humidity, 2);
  display.println(" %");

  display.display();

  // Log data to Serial
  Serial.printf("Time: %s | %.1f F | %.1f C | %.1f %%\n", timeStr, temperatureF, temperatureC, humidity);

  delay(1000); // Update once per second
}
