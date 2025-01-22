# ESP32 Multi-Sensor + PWM Buzzer Project

**Author:** (Your Name)  
**Date:** (Project Date)  
**Board:** ESP32 Dev Module (e.g., `esp32dev` in PlatformIO)

## Overview

This project reads **four sensors** using an ESP32:

1. **TDS Sensor** (Pin 34): Measures total dissolved solids in water, outputting a TDS value in ppm.
2. **Turbidity Sensor** (Pin 35): Estimates how cloudy the water is, outputting a turbidity value in NTU.
3. **Flow Meters** (Pins 16 and 17): Each measures water flow via pulses from a YFS-302 sensor. The software calculates liters/min (L/min).
4. **Temperature** (Pin 4): A DS18B20 digital temperature sensor returning water temperature in Celsius.

Additionally, a **PWM buzzer** on **Pin 26** allows single-frequency or beep toggling modes via hardware PWM.


**Key Points**:

- **Data.h** – Contains pin assignments and `SensorData` struct.  
- **SensorManager** – Encapsulates all sensor reading and updates.  
- **main.cpp** – Initializes everything in `setup()`, calls `sensorManager.update()` frequently in `loop()`, and handles (optional) user commands for the buzzer.

---

## How It Works

1. **PlatformIO Setup**  
   - The `platformio.ini` file specifies libraries:  
     - [OneWire](https://github.com/PaulStoffregen/OneWire), [DallasTemperature](https://github.com/milesburton/Arduino-Temperature-Control-Library)  
     - [FlowSensor](https://github.com/hafidhh/FlowSensor-Arduino)  
     - The ESP32 framework settings.  
   - This automates downloading and installing the correct versions.

2. **SensorManager**  
   - **begin()**: Initializes pins, attaches interrupts for flow meters, sets up ADC resolution for TDS, and starts DS18B20.  
   - **update()**: Called repeatedly in `loop()`; reads TDS, turbidity, temperature, and computes flow rates every 1 second using `millis()` checks (non-blocking).  
   - **FlowSensor** library is used to track pulses in interrupts (`.count()`) and then `.read(…)` to calculate flow in L/min. 

3. **TDS & Turbidity**  
   - Read from analog pins (`TDS_PIN`, `TURBIDITY_PIN`), scale ADC to voltage, and apply placeholder formulas to get ppm or NTU.  
   - In reality, these require calibration with known reference solutions.

4. **Temperature**  
   - Uses DallasTemperature + OneWire on pin `4`. Periodically calls `tempSensor.requestTemperatures()` and fetches the reading in °C.

5. **PWM Buzzer**  
   - Pin `26` is driven by ESP32’s LEDC hardware.  
   - In the code, you can set a single tone frequency (`freq:1234`) or beep mode toggling between two frequencies.  
   - The beep toggling uses a `millis()`-based timer, so the loop remains free of blocking delays.

---

## Usage

1. **Connect the Sensors**  
   - **YFS-302 Flow Meters**: Typically 5 V power, output signal to pins `16` & `17`. Use caution if the ESP32 needs 3.3 V logic or a level shifter.  
   - **TDS & Turbidity**: Output to analog pins `34` & `35`, respectively.  
   - **DS18B20**: Data pin `4` with a 4.7 kΩ pull-up resistor (common arrangement).  
   - **Buzzer**: Connect to pin `26`, with ground shared. Optionally use a transistor if current draw is higher.

2. **Build & Upload** with PlatformIO  
   - `pio run --target upload`  
   - Monitor serial: `pio device monitor`

3. **Observe Data**  
   - The code prints TDS (ppm), turbidity (NTU), temperature (°C), and flow rates (L/min) each cycle.  
   - If you enable buzzer commands in `main.cpp`, type them in the serial monitor (e.g., `off`, `freq:1000`, or `beep:500:1000`).

---

## Calibration Tips

1. **TDS**:  
   - The `calibrationFactor` is a rough guess. Adjust by measuring known ppm solutions.  
2. **Turbidity**:  
   - The polynomial is also an approximation. Actual calibration with known turbidity standards is advised.  
3. **Flow**:  
   - `#define YFS_302 1500` might be off if your measured flow doesn’t match reality. For instance, many YFS-302 are closer to ~450 pulses/liter. Confirm by capturing pulses for exactly 1 L of water.