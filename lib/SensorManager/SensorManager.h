#ifndef SENSORMANAGER_H
#define SENSORMANAGER_H

#include <Arduino.h>
#include <Data.h>  // Ensure correct include

class SensorManager
{
public:
    SensorManager();
    void begin();
    void update();

    // Returns a copy of the current sensor data
    SensorData getSensorData();

    // Optional: Methods to control the relay (pump)
    void setPumpOn();
    void setPumpOff();
    bool isPumpOn();

private:
    SensorData _data;

    // Shared counters for flow meters (must be volatile for ISR)
    static volatile unsigned long _flowCounter1;
    static volatile unsigned long _flowCounter2;

    // Relay state
    bool _pumpIsOn = false;

    // Interrupt Service Routines
    static void IRAM_ATTR flowMeter1ISR();
    static void IRAM_ATTR flowMeter2ISR();

    // Internal methods to perform sensor reads
    void readTDS();
    void readTurbidity();
    void readTemperature();
    void calculateFlowRates();
};

#endif