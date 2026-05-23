#include <Arduino.h>
#include <Wire.h>
#include <time.h>

#include <WiFi.h>
#include <Adafruit_AHTX0.h>
#include <SparkFun_ENS160.h>

#define USER_SETUP_LOADED
#include "../library files/User_Setup.h"
#include <TFT_eSPI.h>

constexpr char WIFI_SSID[] = "OnePlus13";
constexpr char WIFI_PASSWORD[] = "tastyninjin";
constexpr char TIME_ZONE[] = "UTC-1";
constexpr char NTP_SERVER_1[] = "pool.ntp.org";
constexpr char NTP_SERVER_2[] = "time.nist.gov";

constexpr uint8_t I2C_SDA_PIN = 21;
constexpr uint8_t I2C_SCL_PIN = 22;
constexpr uint8_t TOUCH_PIN = 27;

constexpr uint32_t WIFI_TIMEOUT_MS = 15000;
constexpr uint32_t TIME_POLL_MS = 1000;
constexpr uint32_t INITIAL_SENSOR_POLL_MS = 60UL * 1000UL;
constexpr uint32_t SENSOR_POLL_MS = 10UL * 60UL * 1000UL;
constexpr uint32_t TOUCH_DEBOUNCE_MS = 250;

constexpr uint8_t PAGE_AIR_QUALITY = 0;
constexpr uint8_t PAGE_TEMP_HUMIDITY = 1;

TFT_eSPI tft;
Adafruit_AHTX0 aht;
SparkFun_ENS160 ens160;
bool ensInitialized = false;

struct SensorData {
  float temperatureC = NAN;
  float humidityPercent = NAN;
  uint16_t tvocPpb = 0;
  uint16_t eco2Ppm = 0;
  uint8_t iaq = 0;
  bool ahtValid = false;
  bool ensValid = false;
};

SensorData sensorData;

uint8_t currentPage = PAGE_AIR_QUALITY;
uint8_t lastDrawnPage = 255;

char lastTimeText[6] = "";
bool clockReady = false;

uint32_t lastTimePollMs = 0;
uint32_t lastSensorPollMs = 0;
uint32_t lastTouchToggleMs = 0;

bool lastTouchLevel = LOW;

void drawTimeValue(const char *timeText) {
  tft.fillRect(16, 80, 208, 48, TFT_BLACK);
  tft.setTextDatum(MC_DATUM);
  tft.setTextColor(TFT_BLUE, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString(timeText, 120, 104, 2);
  tft.setTextSize(1);
}

void drawWiFiDot() {
  const uint16_t dotColor = (WiFi.status() == WL_CONNECTED) ? TFT_GREEN : TFT_RED;
  tft.fillCircle(210, 56, 6, dotColor);
  tft.drawCircle(210, 56, 6, TFT_DARKGREY);
}

void centerText(const char *text, int32_t x, int32_t y, uint8_t font, uint16_t textColor, uint16_t backgroundColor) {
  tft.setTextColor(textColor, backgroundColor);
  tft.drawString(text, x, y, font);
}

void drawStatusScreen(const char *line1, const char *line2) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ESP32 Air Monitor", 120, 28, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawRoundRect(12, 58, 216, 174, 10, TFT_DARKGREY);

  centerText(line1, 120, 118, 4, TFT_YELLOW, TFT_BLACK);
  centerText(line2, 120, 156, 2, TFT_LIGHTGREY, TFT_BLACK);
}

bool connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  const uint32_t startMs = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - startMs < WIFI_TIMEOUT_MS) {
    delay(250);
  }

  return WiFi.status() == WL_CONNECTED;
}

bool syncClock() {
  configTzTime(TIME_ZONE, NTP_SERVER_1, NTP_SERVER_2);

  struct tm timeInfo {};
  const uint32_t startMs = millis();
  while (!getLocalTime(&timeInfo) && millis() - startMs < WIFI_TIMEOUT_MS) {
    delay(250);
  }

  return getLocalTime(&timeInfo);
}

bool initSensors() {
  bool ok = true;

  if (!aht.begin()) {
    ok = false;
  }

  if (ens160.begin(Wire, ENS160_ADDRESS_HIGH)) {
    ensInitialized = true;
  } else if (ens160.begin(Wire, ENS160_ADDRESS_LOW)) {
    ensInitialized = true;
  } else {
    ensInitialized = false;
    ok = false;
  }

  if (ensInitialized) {
    // Keep ENS160 in standard operation mode.
    ens160.setOperatingMode(SFE_ENS160_STANDARD);
  }

  if (clockReady && lastTimeText[0] != '\0') {
    drawTimeValue(lastTimeText);
  } else {
    drawTimeValue("--:--");
  }

  drawWiFiDot();

  return ok;
}

void drawHeader(const char *pageTitle) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ESP32 Air Monitor", 120, 20, 4);

  tft.drawRoundRect(8, 42, 224, 270, 10, TFT_DARKGREY);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(pageTitle, 120, 56, 2);

  tft.drawFastHLine(20, 72, 200, TFT_DARKGREY);

  drawTimeValue("--:--");
}

void drawMetricRow(const char *label, const char *value, int32_t y, uint16_t color) {
  tft.fillRect(20, y - 12, 200, 28, TFT_BLACK);
  tft.setTextDatum(ML_DATUM);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(label, 24, y, 2);
  tft.setTextDatum(MR_DATUM);
  tft.setTextColor(color, TFT_BLACK);
  tft.drawString(value, 216, y, 2);
}

void renderPageTemplate() {
  if (currentPage == PAGE_AIR_QUALITY) {
    drawHeader("Page 1 - Air Quality");
    drawMetricRow("IAQ (1-5)", "--", 160, TFT_YELLOW);
    drawMetricRow("TVOC", "-- ppb", 196, TFT_GREEN);
    drawMetricRow("eCO2", "-- ppm", 232, TFT_ORANGE);
  } else {
    drawHeader("Page 2 - Climate");
    drawMetricRow("Temperature", "-- C", 160, TFT_GREENYELLOW);
    drawMetricRow("Humidity", "-- %RH", 196, TFT_SKYBLUE);
  }

  if (clockReady && lastTimeText[0] != '\0') {
    drawTimeValue(lastTimeText);
  }

  drawWiFiDot();
}

void updateClockDisplay() {
  struct tm timeInfo {};
  if (!getLocalTime(&timeInfo)) {
    clockReady = false;
    return;
  }

  clockReady = true;

  char currentTime[6];
  strftime(currentTime, sizeof(currentTime), "%H:%M", &timeInfo);

  if (strcmp(currentTime, lastTimeText) == 0 && lastDrawnPage == currentPage) {
    return;
  }

  strcpy(lastTimeText, currentTime);

  drawTimeValue(lastTimeText);
  drawWiFiDot();
}

void updateAirQualityPage() {
  char line[20];

  if (sensorData.ensValid) {
    snprintf(line, sizeof(line), "%u", sensorData.iaq);
  } else {
    snprintf(line, sizeof(line), "N/A");
  }
  drawMetricRow("IAQ (1-5)", line, 160, TFT_YELLOW);

  if (sensorData.ensValid) {
    snprintf(line, sizeof(line), "%u ppb", sensorData.tvocPpb);
  } else {
    snprintf(line, sizeof(line), "N/A");
  }
  drawMetricRow("TVOC", line, 196, TFT_GREEN);

  if (sensorData.ensValid) {
    snprintf(line, sizeof(line), "%u ppm", sensorData.eco2Ppm);
  } else {
    snprintf(line, sizeof(line), "N/A");
  }
  drawMetricRow("eCO2", line, 232, TFT_ORANGE);
}

void updateClimatePage() {
  char line[20];

  if (sensorData.ahtValid) {
    snprintf(line, sizeof(line), "%.1f C", sensorData.temperatureC);
  } else {
    snprintf(line, sizeof(line), "N/A");
  }
  drawMetricRow("Temperature", line, 160, TFT_GREENYELLOW);

  if (sensorData.ahtValid) {
    snprintf(line, sizeof(line), "%.1f %%RH", sensorData.humidityPercent);
  } else {
    snprintf(line, sizeof(line), "N/A");
  }
  drawMetricRow("Humidity", line, 196, TFT_SKYBLUE);

}

void updatePageData() {
  if (currentPage == PAGE_AIR_QUALITY) {
    updateAirQualityPage();
  } else {
    updateClimatePage();
  }
}

void pollSensors() {
  sensors_event_t humidityEvent;
  sensors_event_t tempEvent;
  bool ahtOk = false;
  bool ensOk = false;

  if (aht.getEvent(&humidityEvent, &tempEvent)) {
    sensorData.temperatureC = tempEvent.temperature;
    sensorData.humidityPercent = humidityEvent.relative_humidity;
    sensorData.ahtValid = true;
    ahtOk = true;
  } else {
    sensorData.ahtValid = false;
  }

  if (ensInitialized && sensorData.ahtValid) {
    ens160.setTempCompensationCelsius(sensorData.temperatureC);
    ens160.setRHCompensationFloat(sensorData.humidityPercent);
  }

  if (ensInitialized) {
    const uint8_t ensStatus = ens160.getFlags();
    if (ensStatus != 0xFF && ensStatus != 3) {
      sensorData.tvocPpb = ens160.getTVOC();
      sensorData.eco2Ppm = ens160.getECO2();
      sensorData.iaq = ens160.getAQI();
      sensorData.ensValid = true;
      ensOk = true;
    }
  }

  if (!ensOk) {
    sensorData.ensValid = false;
  }

}

void handleTouchButton() {
  const bool touchLevel = digitalRead(TOUCH_PIN) == HIGH;

  if (touchLevel && !lastTouchLevel && millis() - lastTouchToggleMs > TOUCH_DEBOUNCE_MS) {
    currentPage = (currentPage == PAGE_AIR_QUALITY) ? PAGE_TEMP_HUMIDITY : PAGE_AIR_QUALITY;
    lastTouchToggleMs = millis();
  }

  lastTouchLevel = touchLevel;
}

void setup() {
  pinMode(TOUCH_PIN, INPUT);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  drawStatusScreen("WiFi connecting...", "Please wait");

  if (!connectWiFi()) {
    drawStatusScreen("WiFi failed", "Continuing offline");
  }

  if (WiFi.status() == WL_CONNECTED) {
    drawStatusScreen("NTP syncing...", "Please wait");

    if (!syncClock()) {
      drawStatusScreen("NTP sync failed", "Using uptime only");
    }
  }

  drawStatusScreen("Sensors init...", "Please wait");

  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);
  const bool sensorInitOk = initSensors();
  (void)sensorInitOk;

  // Wait one minute before the first sensor sample, then keep the 10-minute cadence.
  lastSensorPollMs = millis() - (SENSOR_POLL_MS - INITIAL_SENSOR_POLL_MS);

  renderPageTemplate();
  lastDrawnPage = currentPage;

  // System ready (serial logging removed)
}

void loop() {
  handleTouchButton();

  if (currentPage != lastDrawnPage) {
    renderPageTemplate();
    lastDrawnPage = currentPage;
  }

  const uint32_t nowMs = millis();

  if (nowMs - lastTimePollMs >= TIME_POLL_MS) {
    lastTimePollMs = nowMs;
    updateClockDisplay();
  }

  if (nowMs - lastSensorPollMs >= SENSOR_POLL_MS) {
    lastSensorPollMs = nowMs;
    pollSensors();
    updatePageData();
  }
}
