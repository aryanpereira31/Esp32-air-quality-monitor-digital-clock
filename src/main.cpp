#include <Arduino.h>
#include <time.h>

#include <WiFi.h>

#define USER_SETUP_LOADED
#include "../library files/SPI user setup/User_Setup.h"
#include <TFT_eSPI.h>

constexpr char WIFI_SSID[] = "SKYSBUBQ";
constexpr char WIFI_PASSWORD[] = "Dwy3ye5QaSpk";
constexpr char TIME_ZONE[] = "UTC0";
constexpr char NTP_SERVER_1[] = "pool.ntp.org";
constexpr char NTP_SERVER_2[] = "time.nist.gov";

constexpr uint32_t SCREEN_REFRESH_MS = 1000;
constexpr uint32_t WIFI_TIMEOUT_MS = 15000;

TFT_eSPI tft;

void centerText(const char *text, int32_t x, int32_t y, uint8_t font, uint16_t textColor, uint16_t backgroundColor) {
  tft.setTextColor(textColor, backgroundColor);
  tft.drawString(text, x, y, font);
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

void drawStatusScreen(const char *line1, const char *line2) {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ESP32 Network Clock", 120, 28, 4);

  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawRoundRect(12, 58, 216, 174, 10, TFT_DARKGREY);

  centerText(line1, 120, 118, 4, TFT_YELLOW, TFT_BLACK);
  centerText(line2, 120, 156, 2, TFT_LIGHTGREY, TFT_BLACK);
}

void drawDashboardFrame() {
  tft.fillScreen(TFT_BLACK);
  tft.setTextDatum(MC_DATUM);

  tft.setTextColor(TFT_CYAN, TFT_BLACK);
  tft.drawString("ESP32 Network Clock", 120, 24, 4);

  tft.drawRoundRect(12, 52, 216, 206, 10, TFT_DARKGREY);
}

void drawDashboard(const struct tm &timeInfo) {
  tft.setTextDatum(MC_DATUM);

  char timeText[16];
  char dateText[24];

  strftime(timeText, sizeof(timeText), "%H:%M:%S", &timeInfo);
  strftime(dateText, sizeof(dateText), "%a, %d %b %Y", &timeInfo);

  tft.fillRect(16, 66, 208, 64, TFT_BLACK);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.setTextSize(3);
  tft.drawString(timeText, 120, 100, 2);
  tft.setTextSize(1);

  tft.fillRect(24, 144, 192, 24, TFT_BLACK);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);
  tft.drawString(dateText, 120, 164, 2);

  tft.fillRect(24, 196, 192, 24, TFT_BLACK);
  tft.setTextColor(TFT_LIGHTGREY, TFT_BLACK);
  tft.drawString(WiFi.localIP().toString(), 120, 214, 2);
}

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  tft.fillScreen(TFT_BLACK);

  drawStatusScreen("WiFi connecting...", "Please wait");

  if (!connectWiFi()) {
    drawStatusScreen("WiFi failed", "Check credentials");
    Serial.println("WiFi connection failed");
    return;
  }

  if (!syncClock()) {
    drawStatusScreen("NTP sync failed", "Check internet");
    Serial.println("NTP sync failed");
    return;
  }

  drawDashboardFrame();

  Serial.println("Clock synced");
}

void loop() {
  static uint32_t lastUpdateMs = 0;

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (millis() - lastUpdateMs < SCREEN_REFRESH_MS) {
    return;
  }

  lastUpdateMs = millis();

  struct tm timeInfo {};
  if (!getLocalTime(&timeInfo)) {
    drawStatusScreen("Time not ready", "Waiting for NTP");
    Serial.println("Time not ready");
    return;
  }

  drawDashboard(timeInfo);

  Serial.printf("Time: %02d:%02d:%02d\n", timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec);
}