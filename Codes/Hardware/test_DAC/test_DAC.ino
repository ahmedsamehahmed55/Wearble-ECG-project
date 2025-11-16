#include <Wire.h>
#include <Adafruit_MCP4725.h>

Adafruit_MCP4725 dac;
const int vinPin = A2;         // ADC input to read back the waveform
const float vref = 1;        // ADC reference voltage (Arduino supply)

void setup() {
   pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
digitalWrite(3, LOW);
digitalWrite(4, LOW);
digitalWrite(5, LOW);
digitalWrite(6, LOW);
  Serial.begin(9600);
  dac.begin(0x60);  // Change if your scanner showed a different address

  float vout = 1.65;
  uint16_t val = (vout / vref) * 4095;  // DAC is 12-bit, so 4095 is correct here
  dac.setVoltage(val, false);

  Serial.println("DAC output set to 1.65V");
}

void loop() {
    digitalWrite(2, LOW);

  int adcVal = analogRead(vinPin);               // 10-bit ADC: range 0–1023
  float vin = adcVal * (vref / 1023.0);          // Correct scaling for 10-bit

  Serial.print("V_in: ");
  Serial.println(vin, 3);
  
}
