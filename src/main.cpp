#include <Arduino.h>
#include "SensorManager.h"
#include <Data.h>

// Create a global SensorManager
SensorManager sensorManager;

void setup()
{
    Serial.begin(115200);
    sensorManager.begin();

    Serial.println("System Initialized. Relay OFF, sensors ready.");
}

void loop()
{
    // Update sensor readings (assume ~1 second between loops)
    sensorManager.update();

    // Fetch the latest sensor data
    SensorData data = sensorManager.getSensorData();

    // Print the values
/*     Serial.print("TDS (ppm): ");
    Serial.print(data.tdsValue); */
    Serial.print("  |  Temp (C): ");
    Serial.println(data.temperature);

    // Example logic to control pump:
    // Turn pump on if TDS < 100 ppm (arbitrary logic)
    // Turn pump off if TDS >= 100 ppm
    /* if (data.tdsValue < 100.0f && !sensorManager.isPumpOn())
    {
        sensorManager.setPumpOn();
        Serial.println("Pump turned ON");
    }
    else if (data.tdsValue >= 100.0f && sensorManager.isPumpOn())
    {
        sensorManager.setPumpOff();
        Serial.println("Pump turned OFF");
    } */

    // Delay ~1 second
    delay(10);
}