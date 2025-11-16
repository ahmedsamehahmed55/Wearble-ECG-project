 /*
 Author: Ahmed Ahmed
 Date: September 2025
 Project: Impedance Measurement Device
 Microcontroller: Arduino Nano 33 IoT

 Pin assignments and functions:

 | Pin Number | Function                                     |
 |------------|----------------------------------------------|
 | D2         | 200 kΩ resistor (control)                   |
 | D3         | 200 kΩ resistor (control)                   |
 | D4         | 47 kΩ resistor (control)                    |
 | D5         | 47 kΩ resistor (control)                    |
 | D6         | 6.8 kΩ resistor (control)                   |
 | D7         | 6.8 kΩ resistor (control)                   |
 | D10        | Button (manual range selection)             |
 | D9         | OLED display (always HIGH, power enable)    |
 | A0         | DAC (built-in, analog signal generation)    |
 | A1         | ADC (built-in, voltage measurement)         |
 | A4         | SDA (I²C, OLED display)                     |
 | A5         | SCL (I²C, OLED display)                     |
 | Vin        | 9V battery VCC                              |
 | GND        | Ground (from 9V battery)                    |

 Range selection criteria:

 | Status | Resistor Value | Measurement Range    |
 |--------|----------------|----------------------|
 | #1     | 200 kΩ         | 13.8 kΩ – 1 MΩ       |
 | #2     | 47 kΩ          | 3.2 kΩ – 235 kΩ      |
 | #3     | 6.8 kΩ         | 219 Ω – 34 kΩ        |

 Algorithm for automatic range selection:

 - Start with Status = 3
 - If Status = 3 and Z > 15 kΩ → switch to Status = 2
 - If Status = 2 and Z > 200 kΩ → switch to Status = 1
 - If Status = 2 and Z < 10 kΩ → switch to Status = 3
 - If Status = 1 and Z ≤ 150 kΩ → switch to Status = 2
*/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>
#include <Adafruit_SSD1306.h>
Adafruit_SH1106G display(128, 64, &Wire, -1); // Width, Height, Wire, Reset pin (-1 = no reset)

#define dac_builtin A0
#define adc_builtin A1
const float vref = 3.3;
const int waveFreq = 20;
const int samplesPerCycle = 100;
const int sampleRate = waveFreq * samplesPerCycle; // 2000 samples/sec
const int samplePeriod = 1000000 / sampleRate;     // in microseconds

unsigned long lastMicros = 0;
float sineTable[samplesPerCycle];

const int vin_samples_number = 200;
float vin_values[vin_samples_number];
float vin_max[5];

int sampleIndex = 0;
int vin_samples = 0;
int vin_max_samples = 0;

int status = 3;

void setup() {
      delay(1000
      ); // wait 1 sec for OLED to power up

  reset_gpio();

  if (!display.begin( 0x3C)) {  // <-- fixed line
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  // Set GPIO pins based on starting status (status = 3)
  set_gpio(status);
      display.begin();
//display.setTextColor(BLUE);        

 display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);

  Serial.begin(9600);
 // while (!Serial); // Wait for Serial Monitor to open

  analogWriteResolution(10); // 10-bit DAC

  // Precompute sine wave table (10-bit values: 0–1023)
  for (int i = 0; i < samplesPerCycle; i++) {
    sineTable[i] = (uint16_t)(512 + 511 * sin(2 * PI * i / samplesPerCycle));
  }
}

void loop() {
  // Check for incoming serial (manual override input)
  if (Serial.available()) {
    String input = Serial.readString();
    int value = input.toInt();
    if (value >= 1 && value <= 3) {
      status = value;
      set_gpio(status);
      Serial.print("Manually set status to: ");
      Serial.println(status);
    }
  }

  // Generate sine wave and sample ADC
  unsigned long now = micros();
  if (now - lastMicros >= samplePeriod) {
    lastMicros = now;

    analogWrite(dac_builtin, sineTable[sampleIndex]);
    delayMicroseconds(10); // Allow DAC to settle
    int adc_value = analogRead(adc_builtin);
    float voltage = adc_value * (vref / 1023.0) - 1.65;

    vin_values[vin_samples] = voltage;
    vin_samples++;
    if (vin_samples >= vin_samples_number) {
      vin_samples = 0;
      z_calculation();
    }

    

    sampleIndex++;
    if (sampleIndex >= samplesPerCycle) sampleIndex = 0;
  }
}

// GPIO control for range switching
void set_gpio(int value) {
  reset_gpio(); // Reset all to INPUT first

  switch (value) {
    case 1: // 200k
      pinMode(2, OUTPUT); digitalWrite(2, HIGH);
      pinMode(3, OUTPUT); digitalWrite(3, LOW);
      break;
    case 2: // 47k
      pinMode(4, OUTPUT); digitalWrite(4, HIGH);
      pinMode(5, OUTPUT); digitalWrite(5, LOW);
      break;
    case 3: // 6.8k
      pinMode(6, OUTPUT); digitalWrite(6, HIGH);
      pinMode(7, OUTPUT); digitalWrite(7, LOW);
      break;
  }
}

void reset_gpio() {
  for (int i = 2; i <= 7; i++) {
    pinMode(i, INPUT);
  }
}

void z_calculation() {
  // Select resistor based on current range
  double z_bottom = 0.0;
  if (status == 3) z_bottom = 6800.0 / 2.0;
  else if (status==2) z_bottom = 47000.0 / 2.0;
  else if (status == 1) z_bottom = 200000.0 / 2.0;

  double vin_min = 5.0; // Initialize with high value for the min() function
  for (int i = 0; i < vin_samples_number; i++) {
    vin_min = min(vin_values[i], vin_min);
  }

  double z = abs(vin_min) * 2.0;
    //Serial.print("abs(v_in) = ");
 // Serial.println(z); 
  if ( z<= 0.2) { // Avoid divide-by-zero or invalid Z
Serial.println("Out of Range");
 display.clearDisplay();
     display.setCursor(42, 15);
    display.println("OPEN ");
    display.println("");
         display.setCursor(25, 40);
     display.println("CIRCUIT");
    display.display();
return;  
  } 
  /// I need to fix the situtation in whcih the circuit is open !!! 
  z = z / z_bottom;
  z = 3.3 / z;
  z = z - z_bottom;

  vin_max[vin_max_samples] = z;
  vin_max_samples++;

  if (vin_max_samples >= 5) {
    double z_filtered = vin_max[0];
    for (int i = 1; i < 5; i++) {
      z_filtered = max(vin_max[i], z_filtered);
    }

    Serial.print("Z=");
    Serial.print(z_filtered, 2);
    Serial.println(" Ohms");
   display.clearDisplay();
     display.setCursor(26, 15);
     if (z_filtered>1000){
    display.print("");
    display.println(z_filtered/1000.0,3);
      //display.setTextSize(1.2);
     display.setCursor(35, 40);
        display.println(" kohm");

     }
      else if (z_filtered<100){
Serial.println("Short circuit");
 display.clearDisplay();
     display.setCursor(38, 15);
    display.println("SHORT ");
    display.println("");
         display.setCursor(25, 40);
     display.println("CIRCUIT");
    display.display();


     }
     else {
display.setCursor(26, 15);
 display.print("");
    display.println(z_filtered,3);
      //display.setTextSize(1.2);
     display.setCursor(39, 40);

        display.println(" ohm");

     }

    display.display();
    vin_max_samples = 0;

    // Auto range switching
    int newStatus = updateRange(z_filtered, status);
    if (newStatus != status) {
      status = newStatus;
      set_gpio(status);
      Serial.print("Auto-switched to status: ");
      Serial.println(status);
    }
  }
}

int updateRange(float Z, int currentStatus) {
  switch (currentStatus) {
    case 3: // Lowest range
      if (Z > 15000) return 2;
      break;
    case 2:
      if (Z > 200000) return 1; // if more than 200 k ohm, switch to state 1
      else if (Z < 10000) return 3; // if less than 10 k ohm, switch to state 3
      break;
    case 1:
      if (Z <= 150000) return 2; 
      break;
  }
  return currentStatus;
}