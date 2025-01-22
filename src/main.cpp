#include <Arduino.h>
#include "SensorManager.h"
#include <Data.h>

// Buzzer pin
static const int BUZZER_PIN = 26;

// LEDC channel and settings for ESP32
static const int PWM_CHANNEL = 0;    // channel 0..15
static const int PWM_RES     = 8;    // resolution in bits => duty 0..255
static const int DEFAULT_FREQ= 1000; // default frequency in Hz

// We’ll keep track of buzzer modes
enum BuzzerMode {
  MODE_OFF,
  MODE_SINGLE,   // single continuous frequency
  MODE_BEEP      // toggling between two freq
};

BuzzerMode buzzerMode = MODE_OFF;

// For MODE_SINGLE
int singleFreqHz = DEFAULT_FREQ;

// For MODE_BEEP
int beepFreq1Hz = 500;
int beepFreq2Hz = 1000;
bool beepToggleState = false;        // which freq is active now
unsigned long beepLastToggle = 0;    // last time we toggled freq
static const unsigned long BEEP_PERIOD = 500; // ms to alternate freq

void setupPWM() {
  // Set up the LEDC PWM for the buzzer
  ledcSetup(PWM_CHANNEL, DEFAULT_FREQ, PWM_RES);
  ledcAttachPin(BUZZER_PIN, PWM_CHANNEL);
  // Start with buzzer off
  ledcWrite(PWM_CHANNEL, 0);
}

void setBuzzerFrequency(int freqHz) {
  if (freqHz <= 0) {
    // Off if invalid freq
    ledcWriteTone(PWM_CHANNEL, 0); 
    ledcWrite(PWM_CHANNEL, 0);
  } else {
    // Set the new tone
    ledcWriteTone(PWM_CHANNEL, freqHz);
    // e.g. 50% duty
    ledcWrite(PWM_CHANNEL, 128); // in 8-bit => 128/255 ~ 50%
  }
}

void stopBuzzer() {
  // Turn buzzer off
  ledcWriteTone(PWM_CHANNEL, 0);
  ledcWrite(PWM_CHANNEL, 0);
}

// Updates buzzer output each loop based on the current mode
void updateBuzzer() {
  switch (buzzerMode) {
    case MODE_OFF:
      // ensure off
      stopBuzzer();
      break;

    case MODE_SINGLE:
      // singleFreqHz is already set, ensure we have that frequency
      setBuzzerFrequency(singleFreqHz);
      break;

    case MODE_BEEP: {
      // every 500ms, alternate beepToggleState
      unsigned long now = millis();
      if (now - beepLastToggle >= BEEP_PERIOD) {
        beepToggleState = !beepToggleState;
        beepLastToggle = now;
        // pick the current freq
        int activeFreq = beepToggleState ? beepFreq1Hz : beepFreq2Hz;
        setBuzzerFrequency(activeFreq);
      }
      break;
    }
  }
}

SensorManager sensorManager;

void setup() {
    Serial.begin(115200);
    sensorManager.begin();

    setupPWM();

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
    Serial.print(">flow1:");
    Serial.println(data.flowRate1);
    Serial.print(">flow2:");
    Serial.println(data.flowRate2);
    Serial.print(">flowDiff:");
    Serial.println(data.flowRateDiff);
    /* Serial.print(">flowCount1:");
    Serial.println(data.flowCount1);
    Serial.print(">flowCount2:");
    Serial.println(data.flowCount2); */

    // 1) Poll user commands from serial
  /* if (Serial.available() > 0) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input.equalsIgnoreCase("off")) {
      buzzerMode = MODE_OFF;
      Serial.println("Buzzer OFF.");
    }
    else if (input.startsWith("freq:")) {
      // freq:XXXX
      String freqStr = input.substring(5);
      int freqVal = freqStr.toInt();
      if (freqVal > 0) {
        singleFreqHz = freqVal;
        buzzerMode = MODE_SINGLE;
        Serial.print("Single frequency mode: ");
        Serial.print(freqVal);
        Serial.println(" Hz");
      } else {
        Serial.println("Invalid freq command or zero freq. Doing off.");
        buzzerMode = MODE_OFF;
      }
    }
    else if (input.startsWith("beep:")) {
      // beep:freq1:freq2
      // parse freq1 & freq2
      // find first colon after "beep:"
      int firstColonIndex = input.indexOf(':', 5); // start searching after "beep:"
      if (firstColonIndex == -1) {
        Serial.println("Invalid beep command. Usage: beep:freq1:freq2");
      } else {
        String f1Str = input.substring(5, firstColonIndex);
        String f2Str = input.substring(firstColonIndex + 1);
        int f1Val = f1Str.toInt();
        int f2Val = f2Str.toInt();

        if (f1Val > 0 && f2Val > 0) {
          beepFreq1Hz = f1Val;
          beepFreq2Hz = f2Val;
          buzzerMode = MODE_BEEP;
          beepToggleState = false;  // so next toggle picks freq1 first
          beepLastToggle = millis(); // reset toggle timer
          Serial.print("Beep mode: freq1=");
          Serial.print(f1Val);
          Serial.print(" Hz, freq2=");
          Serial.print(f2Val);
          Serial.println(" Hz");
        } else {
          Serial.println("Invalid beep: freq must be > 0");
        }
      }
    }
    else {
      // Unrecognized
      Serial.println("Commands:");
      Serial.println("  off             => turn buzzer off");
      Serial.println("  freq:XXXX       => single frequency in Hz");
      Serial.println("  beep:XXXX:YYYY  => beep between two freqs");
    }
  }

  // 2) Update the buzzer based on mode
  updateBuzzer(); */

}