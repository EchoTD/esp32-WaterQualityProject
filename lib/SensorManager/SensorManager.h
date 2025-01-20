#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include "Data.h"
#include "OneWire.h"
#include "DallasTemperature.h"
//#include "GravityTDS.h"

class SensorManager {
public:
    SensorManager();
    void begin();
    void update();
    SensorData getSensorData();

private:
    SensorData _data;
    OneWire oneWire;
    DallasTemperature tempSensor;
    //GravityTDS tdsSensor;

    // Internal methods to perform sensor reads
    void readTDS();
    void readTurbidity();
    void readTemperature();
    //void calculateFlowRates();

    float ntu;
    float calibrationFactor;
};

#endif