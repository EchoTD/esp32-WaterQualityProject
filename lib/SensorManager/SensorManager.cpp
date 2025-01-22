#include "SensorManager.h"
#include "Data.h"
#include "FlowSensor_Type.h"

#ifndef YFS_302
#define YFS_302 1500
#endif

// Initialize the static pointers to null
FlowSensor* SensorManager::flowMeter1Ptr = nullptr;
FlowSensor* SensorManager::flowMeter2Ptr = nullptr;

SensorManager::SensorManager(): oneWire(TEMP_SENSOR_PIN),
                                tempSensor(&oneWire),
                                flowMeter1(YFS_302, FLOWMETER2_PIN),
                                flowMeter2(YFS_302, FLOWMETER1_PIN),
                                _lastFlowMillis(0) {
    _data.tdsValue       = 0.0f;
    _data.turbidityValue = 0.0f;
    _data.flowRate1      = 0.0f;
    _data.flowRate2      = 0.0f;
    _data.flowRateDiff   = 0.0f;
    _data.flowCount1     = 0;
    _data.flowCount2     = 0;
    _data.temperature    = 0.0f;

    ntu = 0.0f;
    calibrationFactor = 0.0f;
}

void SensorManager::begin() {
    pinMode(TURBIDITY_PIN, INPUT_PULLUP);
    pinMode(RELAY_PIN, OUTPUT);

    flowMeter1Ptr = &flowMeter1;
    flowMeter2Ptr = &flowMeter2;

    // Provide the ISR callback, default to internal pull-up = false => we do pinMode ourselves
    // bool pullup = false => library calls pinMode(this->_pin, INPUT_PULLUP) inside begin()
    flowMeter1.begin(flowMeter1ISR, false); 
    flowMeter2.begin(flowMeter2ISR, false);

    pinMode(TDS_PIN, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    calibrationFactor = 0.000185f;

    tempSensor.begin();
}

void SensorManager::update() {
    readTDS();
    readTurbidity();
    readTemperature();
    readFlowRates();
}

SensorData SensorManager::getSensorData() {
    return _data;
}

void SensorManager::readTDS() {
    int adcValue = analogRead(TDS_PIN);
    float voltage = (adcValue / 4095.0f) * 3.3f; 
    float conductivity = voltage / calibrationFactor;
    float tempCompensation = 25.0f / (_data.temperature + 273.15f);
    float tds = conductivity * tempCompensation;
    _data.tdsValue = tds;
}

void SensorManager::readTurbidity() {
    int adcValue = analogRead(TURBIDITY_PIN);
    float voltage = (adcValue / 4095.0f) * 5.0f;

    // Debug
    /* Serial.println(adcValue);
    Serial.println(voltage); */

    if (voltage < 2.5f) {
        ntu = 3000.0f;
    } else {
        ntu = (-1120.4f * voltage * voltage) + (5742.3f * voltage) - 4352.9f;
        if (ntu < 0) ntu = 0.0f;
    }
    _data.turbidityValue = ntu;
}

void SensorManager::readTemperature() {
    tempSensor.requestTemperatures();
    _data.temperature = tempSensor.getTempCByIndex(0);
}

void SensorManager::readFlowRates() {
    unsigned long now = millis();

    // if at least 1000 ms (1 second) has passed, compute flow
    if (now - _lastFlowMillis >= 1000) {
        _lastFlowMillis = now;

        // library function to compute flow since last read
        // pass '0' as calibration offset if you want
        flowMeter1.read(0);
        flowMeter2.read(0);

        // get L/min
        float flow1 = flowMeter1.getFlowRate_m();  // L/min
        float flow2 = flowMeter2.getFlowRate_m();

        _data.flowRate1 = flow1;
        _data.flowRate2 = flow2;
        _data.flowRateDiff = flow1 - flow2;

        // get pulses if you want
        unsigned long p1 = flowMeter1.getPulse();
        unsigned long p2 = flowMeter2.getPulse();
        _data.flowCount1 = p1;
        _data.flowCount2 = p2;

        // (optional) float vol1 = flowMeter1.getVolume(); // total liters

        // Debug
        /* Serial.print("[Flow update] Flow1: ");
        Serial.print(flow1, 2);
        Serial.print(" L/min, Flow2: ");
        Serial.print(flow2, 2);
        Serial.print(" L/min, Diff: ");
        Serial.print(_data.flowRateDiff, 2);
        Serial.print("  |  Count1: ");
        Serial.print(p1);
        Serial.print("  Count2: ");
        Serial.println(p2); */
    }
}

void IRAM_ATTR SensorManager::flowMeter1ISR() {
    // Just call .count() on flowMeter1
    if (flowMeter1Ptr) {
        flowMeter1Ptr->count();
    }
}

void IRAM_ATTR SensorManager::flowMeter2ISR() {
    // Just call .count() on flowMeter2
    if (flowMeter2Ptr) {
        flowMeter2Ptr->count();
    }
}