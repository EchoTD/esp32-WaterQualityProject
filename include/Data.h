#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

// Pin definitions (adjust these to your wiring)
static const uint8_t TDS_PIN         = 34;  // Analog
static const uint8_t TURBIDITY_PIN   = 35;  // Analog
static const uint8_t FLOWMETER1_PIN  = 25;  // Digital interrupt
static const uint8_t FLOWMETER2_PIN  = 26;  // Digital interrupt
static const uint8_t TEMP_SENSOR_PIN = 4;   // Analog (placeholder if not truly DS18B20)
static const uint8_t RELAY_PIN       = 12;  // Digital out to drive a relay

// Structure to hold all sensor readings
typedef struct
{
    float tdsValue;         // in ppm (for example)
    float turbidityValue;   // in NTU (for example)
    float temperature;      // in °C (placeholder if purely analog)
    float flowRate1;        // in L/min or some unit
    float flowRate2;        // in L/min or some unit
    unsigned long flowCount1; // raw pulse count
    unsigned long flowCount2; // raw pulse count
} SensorData;

#endif