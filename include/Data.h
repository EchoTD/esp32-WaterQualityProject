#ifndef DATA_H
#define DATA_H

#include <Arduino.h>

static const uint8_t TDS_PIN         = 34;
static const uint8_t TURBIDITY_PIN   = 35;
static const uint8_t FLOWMETER1_PIN  = 16;
static const uint8_t FLOWMETER2_PIN  = 17;
static const uint8_t TEMP_SENSOR_PIN = 4;

typedef struct {
    float tdsValue;         // in ppm 
    float turbidityValue;   // in NTU 
    float temperature;      // in °C 
    float flowRate1;        // in L/min 
    float flowRate2;        // in L/min 
    float flowRateDiff;
    unsigned long flowCount1;
    unsigned long flowCount2;
} SensorData;

#endif