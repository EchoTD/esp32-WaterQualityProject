#include "SensorManager.h"
#include "Data.h"

// Define static members
/* volatile unsigned long SensorManager::_flowCounter1 = 0;
volatile unsigned long SensorManager::_flowCounter2 = 0; */

SensorManager::SensorManager(): oneWire(TEMP_SENSOR_PIN), tempSensor(&oneWire) {
    _data.tdsValue       = 0.0f;
    _data.turbidityValue = 0.0f;
    _data.flowRate1      = 0.0f;
    _data.flowRate2      = 0.0f;
    _data.flowCount1     = 0;
    _data.flowCount2     = 0;
    _data.temperature    = 0.0f;

    ntu = 0.0f;
    calibrationFactor = 0.0f;
}

void SensorManager::begin() {
    pinMode(TURBIDITY_PIN,   INPUT_PULLUP);
    pinMode(FLOWMETER1_PIN,  INPUT_PULLUP);
    pinMode(FLOWMETER2_PIN,  INPUT_PULLUP);
    pinMode(RELAY_PIN,       OUTPUT);

    // TDS sensor config
    pinMode(TDS_PIN, INPUT);
    analogReadResolution(12);
    analogSetAttenuation(ADC_11db);
    calibrationFactor = 0.0001f;
    // Temp sensor config
    tempSensor.begin();
    
}

void SensorManager::update() {
    // Read and process sensors
    readTDS();
    readTurbidity();
    readTemperature();
    //calculateFlowRates();
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
    // Basic analog read for Turbidity sensor
    int adcValue = analogRead(TURBIDITY_PIN);
    float voltage = (adcValue / 4095.0f) * 5.0f;
    // Debug
    Serial.println(adcValue);
    Serial.println(voltage);

    if (voltage < 2.5f){
        ntu = 3000;
    } else {
        ntu = ( -1120.4f * voltage * voltage ) + ( 5742.3f * voltage ) - 4352.9f;
        if (ntu < 0) ntu = 0;
    }

    _data.turbidityValue = ntu;
}

void SensorManager::readTemperature() {

    tempSensor.requestTemperatures();
    _data.temperature = tempSensor.getTempCByIndex(0);
}

/* void SensorManager::calculateFlowRates(){
    // Typically, flow sensors produce pulses in proportion to flow rate.
    // Flow = pulses_per_second / calibrationFactor, etc.
    // Since we’re only reading absolute pulse counts in this example,
    // you could measure pulses over a time interval.

    // For demonstration, let’s assume we call `update()` every 1 second.
    // So the difference in pulses over 1 second => pulses_per_second.
    // We'll keep track of the last count to find the delta.

    static unsigned long lastCount1 = 0;
    static unsigned long lastCount2 = 0;

    unsigned long currentCount1 = _flowCounter1;
    unsigned long currentCount2 = _flowCounter2;

    // pulses this second
    unsigned long pulses1 = currentCount1 - lastCount1;
    unsigned long pulses2 = currentCount2 - lastCount2;

    // Update last counts
    lastCount1 = currentCount1;
    lastCount2 = currentCount2;

    // Convert pulses/sec to L/min or other unit based on your sensor’s spec.
    // For example, a typical flow sensor might have a known:
    // 450 pulses per Liter. (just an example)
    // Then pulses/sec => L/sec => L/min => ...
    float pulsesPerLiter = 450.0f; 
    float flowRate1LPS = pulses1 / pulsesPerLiter; // L/sec
    float flowRate2LPS = pulses2 / pulsesPerLiter;

    // Convert L/sec to L/min:
    _data.flowRate1 = flowRate1LPS * 60.0f;
    _data.flowRate2 = flowRate2LPS * 60.0f;
} */