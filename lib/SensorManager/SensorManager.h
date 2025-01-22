#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include "Data.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "FlowSensor.h"

// Forward-declare SensorManager so we can reference it in ISRs:
class SensorManager;

class SensorManager {
public:
    SensorManager();
    void begin();
    void update();
    SensorData getSensorData();

    // Provide static pointers so ISRs can reference the correct FlowSensor
    static FlowSensor* flowMeter1Ptr;
    static FlowSensor* flowMeter2Ptr;

    // Provide static ISRs for each flow meter
    static void IRAM_ATTR flowMeter1ISR();
    static void IRAM_ATTR flowMeter2ISR();

private:
    SensorData _data;

    // DS18B20
    OneWire oneWire;
    DallasTemperature tempSensor;

    // FlowSensors from the code you provided
    FlowSensor flowMeter1;
    FlowSensor flowMeter2;
    unsigned long _lastFlowMillis;

    // Internal sensor reads
    void readTDS();
    void readTurbidity();
    void readTemperature();
    void readFlowRates();

    float ntu;
    float calibrationFactor;
};

#endif