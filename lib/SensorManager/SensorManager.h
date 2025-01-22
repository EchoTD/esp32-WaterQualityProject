#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include "Data.h"
#include "OneWire.h"
#include "DallasTemperature.h"
#include "FlowSensor.h"

class SensorManager;

class SensorManager {
public:
    SensorManager();
    void begin();
    void update();
    SensorData getSensorData();

    static FlowSensor* flowMeter1Ptr;
    static FlowSensor* flowMeter2Ptr;

    static void IRAM_ATTR flowMeter1ISR();
    static void IRAM_ATTR flowMeter2ISR();

private:
    SensorData _data;

    OneWire oneWire;
    DallasTemperature tempSensor;
    FlowSensor flowMeter1;
    FlowSensor flowMeter2;
    
    void readTDS();
    void readTurbidity();
    void readTemperature();
    void readFlowRates();

    unsigned long _lastFlowMillis;
    float ntu;
    float calibrationFactor;
};

#endif