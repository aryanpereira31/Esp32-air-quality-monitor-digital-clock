# Pseudocode for `src/main.cpp`

This document is a readable pseudocode translation of `src/main.cpp`.

## Globals / Configuration
- WiFi credentials, NTP servers
- I2C SDA/SCL pins, touch pin
- `TOUCH_CS` defined as `-1` so TFT_eSPI touch support stays disabled and the warning is silenced
- TIMEOUT and POLL interval constants
- Page identifiers (Air Quality, Climate)

## Objects
- `TFT_eSPI tft` — display
- `Adafruit_AHTX0 aht` — temperature & humidity sensor
- `SparkFun_ENS160 ens160` — gas / air quality sensor

## SensorData struct
- Fields: `temperatureC`, `humidityPercent`, `tvocPpb`, `eco2Ppm`, `iaq`
- Flags: `ahtValid`, `ensValid`

## State variables
- `currentPage`, `lastDrawnPage`, `lastTimeText[]`, `clockReady`
- `lastTimePollMs`, `lastSensorPollMs`, `lastTouchToggleMs`, `lastTouchLevel`

## logSensorCsv()
- Removed: CSV/Serial logging was removed from the implementation; no CSV output is produced.

## Display helpers
- `drawTimeValue(timeText)` — clear time region, draw centered time
- `drawWiFiDot()` — draw green or red dot for WiFi status
- `centerText(...)` — utility for centered text
- `drawStatusScreen(line1, line2)` — show large status message

## connectWiFi()
- Set WiFi mode to station and begin with SSID/password
- Wait up to `WIFI_TIMEOUT_MS` for connection
- Return connected boolean

## syncClock()
- Configure timezone/NTP servers via `configTzTime`
- Wait up to `WIFI_TIMEOUT_MS` for `getLocalTime`
- Return success boolean

## initSensors()
- Initialize AHT sensor; mark failure if not present
- Try ENS160 at two I2C addresses; set `ensInitialized` and operating mode
- Draw initial time placeholder and WiFi dot
- Return overall sensor init success

## UI template and drawing
- `drawHeader(pageTitle)` — draws title, framed content area, and time placeholder
- `drawMetricRow(label, value, y, color)` — draws a metric label + right-aligned value
- `renderPageTemplate()` — render either Air Quality or Climate template, include placeholders and WiFi/time

## Clock update
- `updateClockDisplay()`:
  - Get local time; if unavailable mark `clockReady=false`
  - Format `HH:MM` and only redraw if changed (and page changed)
  - Draw time and WiFi dot

## Page updates
- `updateAirQualityPage()` — format IAQ/TVOC/eCO2 or `N/A` and draw rows
- `updateClimatePage()` — format temperature/humidity or `N/A` and draw rows
- `updatePageData()` — call appropriate page update based on `currentPage`

## Sensor polling
- `pollSensors()`:
  - Read AHT events; update temperature/humidity and `ahtValid`
  - If ENS initialized and AHT valid, set temp/RH compensation on ENS160
  - If ENS initialized and flags valid: read TVOC, eCO2, IAQ and mark `ensValid`
  - Otherwise mark ENS as invalid
  - (No CSV/Serial logging in current implementation)

## Touch handler
- `handleTouchButton()`:
  - Read `TOUCH_PIN` level
  - On rising edge with debounce, toggle `currentPage`

## setup() flow
1. (No Serial console initialization — logging removed)
2. Configure `TOUCH_PIN`
3. Initialize TFT, clear screen
4. Attempt WiFi; show status screens for connecting/failure
5. If WiFi connected, attempt NTP sync; show status if fail
6. Initialize I2C and sensors; show status if sensor init fails
7. Schedule first sensor poll (initial delay) and draw initial template
8. (No CSV/Serial output)

## loop() flow
- Call `handleTouchButton()`
- If `currentPage` changed: `renderPageTemplate()`
- Periodically (every `TIME_POLL_MS`) call `updateClockDisplay()`
- Periodically (every `SENSOR_POLL_MS`) call `pollSensors()` and `updatePageData()`

## Error / fallback behavior
- Continue running with placeholders if WiFi, NTP, or sensors fail
- Display `N/A` for unavailable sensor values

---

File generated from `src/main.cpp` for documentation and quick reference.
