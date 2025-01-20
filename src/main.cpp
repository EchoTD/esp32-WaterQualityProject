#include <Arduino.h>
#include "SensorManager.h"
#include <Data.h>

// Create a global SensorManager
SensorManager sensorManager;

void setup() {
    Serial.begin(115200);
    sensorManager.begin();

    Serial.println("System Initialized.");
}

void loop() {
    sensorManager.update();
    SensorData data = sensorManager.getSensorData();

    /* int adcVal = analogRead(34);
    float voltage = (adcVal / 4095.0f) * 3.3f;
    Serial.print(adcVal);
    Serial.print("\t");
    Serial.print(voltage);
    Serial.println(); */

    Serial.print(">temp_c:");
    Serial.println(data.temperature);
    Serial.print(">ppm:");
    Serial.println(data.tdsValue);
    Serial.print(">ntu:");
    Serial.println(data.turbidityValue);

    //delay(1);
}