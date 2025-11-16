#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac_ext;  // External DAC (MCP4725)

const int dac_builtin = A0;         
const int vinPin = A1;              
const float vref = 3.3;             

const int sampleRate = 1000;        
const int waveFreq = 20;            
const int samplesPerCycle = sampleRate / waveFreq;
float sineTable[samplesPerCycle];   
int sampleIndex = 0;                

unsigned long lastUpdate = 0;
const unsigned long interval = 1000000UL / sampleRate; // µs per sample

void setup() {
  pinMode(2, OUTPUT); digitalWrite(2, LOW); 
  pinMode(3, OUTPUT); digitalWrite(3, LOW);
  pinMode(4, OUTPUT); digitalWrite(4, LOW);
  pinMode(5, OUTPUT); digitalWrite(5, LOW);
  pinMode(6, OUTPUT); digitalWrite(6, LOW);

  Serial.begin(115200);
  while (!Serial && millis() < 2000);

  dac_ext.begin(0x60);  
  float vout = 1.65;
  uint16_t val = (vout / vref) * 4095;
  dac_ext.setVoltage(val, false);
  Serial.println("External DAC (MCP4725) set to 1.65 V");

  analogWriteResolution(10);  // Range 0–1023

  for (int i = 0; i < samplesPerCycle; i++) {
    float angle = 2 * PI * i / samplesPerCycle;
    float normalized = (sin(angle) + 1.0) / 2.0;
    sineTable[i] = normalized * 1023;  
  }
}

void loop() {
  unsigned long now = micros();
  if (now - lastUpdate >= interval) {
    lastUpdate = now;

    analogWrite(dac_builtin, (int)sineTable[sampleIndex]);

    int adcVal = analogRead(vinPin);
    const float vOffset = 1.65;
    float vin = (adcVal * (vref / 1023.0)) - vOffset;
    Serial.print("V_in: ");
    Serial.println(vin, 3);

    sampleIndex = (sampleIndex + 1) % samplesPerCycle;
  }
}
