#  Air Quality Monitor  
ESP32 • ENS160 • AHT21 • 2" TFT LCD 

A compact indoor air‑quality monitoring system using the ENS160 gas sensor and AHT21 temperature/humidity sensor, powered by an ESP32 and displayed on a 2‑inch TFT LCD.

---

##  Components Used

| Category         | Component       | Description                                                                 |
|------------------|-----------------|-----------------------------------------------------------------------------|
| **Sensors**      | **ENS160**      | Multi‑gas sensor measuring VOCs and providing IAQ index.                    |
|                  | **AHT21**       | Temperature & humidity sensor used for environmental compensation.          |
| **Microcontroller** | **ESP32**    | Main controller handling sensor readings and TFT display output.            |
| **Output**       | **2-inch TFT LCD** | Displays IAQ, TVOC, eCO₂, temperature, and humidity.                    |
| **Power**        | **USB‑C (5V)**  | System powered via a standard 5V USB‑C input.                               |

---

## 📝 Description

### Sensors  
The **ENS160** detects VOCs and calculates **IAQ**, **TVOC**, and **eCO₂**.  
The **AHT21** measures **temperature** and **humidity**, providing compensation data for accurate gas readings.

### Microcontroller  
The **ESP32** reads both sensors via I²C, processes the data, and updates the TFT display in real time.

### Display  
A **2‑inch TFT LCD** presents all air‑quality metrics clearly and visually.

### Power  
Powered through a **5V USB‑C** port, compatible with chargers, power banks, and USB ports.

### Capacitive Touch Button — Page Switching

 includes a capacitive touch input connected to the ESP32.  
Touching the pad switches the TFT display between **Page 1** (Air Quality) and **Page 2** (Temperature & Humidity) 

