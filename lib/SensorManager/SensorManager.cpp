#include "SensorManager.h"
#include "Data.h"

// Define static members
volatile unsigned long SensorManager::_flowCounter1 = 0;
volatile unsigned long SensorManager::_flowCounter2 = 0;

SensorManager::SensorManager()
{
    // Initialize fields to zero or default values
    _data.tdsValue       = 0.0f;
    _data.turbidityValue = 0.0f;
    _data.flowRate1      = 0.0f;
    _data.flowRate2      = 0.0f;
    _data.flowCount1     = 0;
    _data.flowCount2     = 0;
    _data.temperature    = 0.0f;
}

void SensorManager::begin()
{
    // Initialize sensor pins
    pinMode(TDS_PIN,         INPUT);
    pinMode(TURBIDITY_PIN,   INPUT);
    pinMode(FLOWMETER1_PIN,  INPUT_PULLUP);
    pinMode(FLOWMETER2_PIN,  INPUT_PULLUP);
    pinMode(TEMP_SENSOR_PIN, INPUT); // For DS18B20, you'd need 1-Wire or library code
    pinMode(RELAY_PIN,       OUTPUT);

    // Default: turn relay off
    digitalWrite(RELAY_PIN, LOW);
    _pumpIsOn = false;

    // Attach interrupts for the flow meters
    attachInterrupt(digitalPinToInterrupt(FLOWMETER1_PIN), flowMeter1ISR, RISING);
    attachInterrupt(digitalPinToInterrupt(FLOWMETER2_PIN), flowMeter2ISR, RISING);
}

void SensorManager::update()
{
    // Read and process sensors
    readTDS();
    readTurbidity();
    readTemperature();
    calculateFlowRates();
}

SensorData SensorManager::getSensorData()
{
    // Copy out the raw counts as well (in case user wants them)
    _data.flowCount1 = _flowCounter1;
    _data.flowCount2 = _flowCounter2;

    return _data;
}

// -------------------- Relay Control --------------------
void SensorManager::setPumpOn()
{
    digitalWrite(RELAY_PIN, HIGH);
    _pumpIsOn = true;
}

void SensorManager::setPumpOff()
{
    digitalWrite(RELAY_PIN, LOW);
    _pumpIsOn = false;
}

bool SensorManager::isPumpOn()
{
    return _pumpIsOn;
}

// -------------------- ISR Functions --------------------
void IRAM_ATTR SensorManager::flowMeter1ISR()
{
    _flowCounter1++;
}

void IRAM_ATTR SensorManager::flowMeter2ISR()
{
    _flowCounter2++;
}

// -------------------- Internal Reads --------------------
void SensorManager::readTDS()
{
    // Basic analog read for TDS sensor
    int adcValue = analogRead(TDS_PIN);
    
    // Convert raw ADC value to voltage [0..3.3V] on ESP32 (12-bit: 0-4095)
    float voltage = (adcValue / 4095.0f) * 3.3f;

    // A *very* rough example TDS formula: TDS (ppm) ~ voltage * someFactor
    // Different sensors have different calibrations. Adjust accordingly.
    float tdsFactor = 400.0f;  // e.g. if 1V ~ 400 ppm (placeholder)
    float tds = voltage * tdsFactor;

    _data.tdsValue = tds;
}

void SensorManager::readTurbidity()
{
    // Basic analog read for Turbidity sensor
    int adcValue = analogRead(TURBIDITY_PIN);
    // Convert to voltage
    float voltage = (adcValue / 4095.0f) * 3.3f;

    // Example formula for a typical turbidimeter. 
    // (This is purely for demonstration; you must calibrate for your sensor.)
    // For instance, one known approximate formula is:
    // NTU ~ -1120.4*(voltage^2) + 5742.3*voltage - 4353.8
    float ntu = -1120.4f * voltage * voltage 
                 + 5742.3f * voltage 
                 - 4353.8f;

    // Clip the value to 0 if it goes negative from formula quirks
    if (ntu < 0) ntu = 0;

    _data.turbidityValue = ntu;
}

void SensorManager::readTemperature()
{
    // If this is truly a DS18B20, you'd do a digital read routine with a library.
    // For demonstration, let's pretend it's an analog thermistor or sensor.

    int adcValue = analogRead(TEMP_SENSOR_PIN);
    // Convert to voltage
    float voltage = (adcValue / 4095.0f) * 5;

    // A simple approximate conversion for a hypothetical linear sensor:
    // e.g. 10 mV/°C offset from 0°C, just as a placeholder. 
    // Obviously, real thermistors need more sophisticated calculation.
    float tempC = (voltage / 0.01f); // If 10 mV = 1°C, then 1V = 100°C

    _data.temperature = tempC;

}

void SensorManager::calculateFlowRates()
{
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
}