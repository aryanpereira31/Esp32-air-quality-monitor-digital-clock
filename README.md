# Esp32-air-quality-monitor-digital-clock
ESP32 indoor monitor using AHT21 (temp/humidity) and ENS160 (TVOC, AQI, eCO₂ estimate). Data is shown on a 2" ST7789 SPI TFT with a clock. A capacitive tap button switches between two pages: Air Quality and Temperature/Humidity.


<img width="541" height="941" alt="workbreakdownstructure drawio" src="https://github.com/user-attachments/assets/d7b25187-b1bf-4229-8dc6-6c5c4858eb01" />

ESP32 • ENS160 • AHT21 • 2" TFT LCD

A compact indoor air‑quality monitoring system using the ENS160 gas sensor and AHT21 temperature/humidity sensor, powered by an ESP32 and displayed on a 2‑inch TFT LCD.

📦 Components Used

Category

Component

Description

Sensors

ENS160

Multi‑gas sensor measuring VOCs and providing IAQ index.



AHT21

Temperature & humidity sensor used for environmental compensation.

Microcontroller

ESP32

Main controller handling sensor readings and TFT display output.

Display

2‑inch TFT LCD

Shows IAQ, TVOC, eCO₂, temperature, and humidity.

Power

USB‑C (5V)

System powered via a standard 5V USB‑C input.

📝 System Description

🧪 Sensors

ENS160 detects VOCs and calculates IAQ, TVOC, and eCO₂.

AHT21 provides temperature and humidity, essential for ENS160 compensation.

🧠 Microcontroller

The ESP32 reads both sensors via I²C, processes the data, and updates the TFT display in real time.

🖥 Display

A 2‑inch TFT LCD is used to present all air‑quality metrics clearly and visually.

🔌 Power

Powered through a 5V USB‑C port, compatible with chargers, power banks, and USB ports.



