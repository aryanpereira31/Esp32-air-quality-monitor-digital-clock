# Air Quality Monitor
ESP32 • ENS160 • AHT21 • 2" TFT LCD

A compact indoor air-quality monitor built with an ESP32, an ENS160 gas sensor, and an AHT21 temperature/humidity sensor. The device shows live readings on a 2-inch TFT LCD and uses a touch input to switch between display pages.

---

## Components Used

| Category | Component | Description |
|---|---|---|
| Sensors | ENS160 | Multi-gas sensor that provides IAQ, TVOC, and eCO2 values. |
| Sensors | AHT21 | Temperature and humidity sensor used for environmental compensation. |
| Microcontroller | ESP32 | Main controller handling Wi-Fi, NTP time sync, sensor reads, and display updates. |
| Display | 2-inch TFT LCD | Shows the clock, sensor values, and page titles. |
| Input | Capacitive touch pin | Switches between the two display pages. |
| Power | USB-C 5V | Standard 5V power input for the system. |

---

## Overview

The ESP32 connects to Wi-Fi, syncs time with NTP, reads both sensors over I2C, and refreshes the TFT screen once per second. The UI is split into a simple status screen during startup and two main pages after the device is ready.

---

## Page Layout

### Startup / Status Screen

| Area | Content |
|---|---|
| Header | Project name: `ESP32 Air Monitor` |
| Main message | Wi-Fi, NTP, or sensor initialization status |
| Frame | Centered status panel with two lines of text |

### Page 1 - Air Quality

| Area | Content |
|---|---|
| Header | `ESP32 Air Monitor` + `Page 1 - Air Quality` |
| Clock | Time shown near the top of the screen |
| Indicator | Wi-Fi status dot in the top-right corner |
| Metric 1 | IAQ value |
| Metric 2 | TVOC in ppb |
| Metric 3 | eCO2 in ppm |

### Page 2 - Climate

| Area | Content |
|---|---|
| Header | `ESP32 Air Monitor` + `Page 2 - Climate` |
| Clock | Time shown near the top of the screen |
| Indicator | Wi-Fi status dot in the top-right corner |
| Metric 1 | Temperature in C |
| Metric 2 | Humidity in %RH |

### Page Switching

Touching the capacitive input toggles between the Air Quality page and the Climate page.

---

## Description

The ENS160 provides air-quality values such as IAQ, TVOC, and eCO2. The AHT21 supplies temperature and humidity readings, which are also used to compensate the gas sensor for better accuracy. All values are rendered on the TFT display in real time.

---

## Build Notes

The TFT setup is configured through the custom User_Setup.h file in the library files folder. The project is built with PlatformIO and uses the TFT_eSPI, SparkFun ENS160, and Adafruit AHTX0 libraries.

---

## Libraries Used

| Library | Purpose |
|---|---|
| [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI) | Drives the 2" TFT display and drawing functions. |
| [SparkFun Indoor Air Quality Sensor - ENS160](https://github.com/sparkfun/SparkFun_Indoor_Air_Quality_Sensor-ENS160_Arduino_Library) | Reads IAQ, TVOC, and eCO2 values from the ENS160 sensor. |
| [Adafruit AHTX0](https://github.com/adafruit/Adafruit_AHTX0) | Reads temperature and humidity from the AHT21 sensor. |
| Arduino core for ESP32 | Provides Wi-Fi, I2C, timing, and serial support used by the firmware. |

