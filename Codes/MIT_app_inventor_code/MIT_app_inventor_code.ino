#include <ArduinoBLE.h>

BLEService LEDService("19b10000-e8f2-537e-4f6c-d104768a1214");

BLECharacteristic redCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);
BLECharacteristic greenCharacteristic("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);
BLECharacteristic blueCharacteristic("19b10003-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);

const int redPIN = 13;
const int greenPIN = 12;
const int bluePIN = 11;
int x=0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);

  digitalWrite(redPIN, LOW);
  digitalWrite(greenPIN, HIGH);
  digitalWrite(bluePIN, HIGH);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("Heart_Monitor");
  BLE.setAdvertisedService(LEDService);

  LEDService.addCharacteristic(redCharacteristic);
  LEDService.addCharacteristic(greenCharacteristic);
  LEDService.addCharacteristic(blueCharacteristic);
  BLE.addService(LEDService);

  BLE.advertise();
  Serial.println("BLE device is advertising...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
Serial.println ("Trying to reconenct");
    while (central.connected()) {
      // Write to LEDs
      if (redCharacteristic.written()) {
        digitalWrite(redPIN, redCharacteristic.value()[0]);
      }
      if (greenCharacteristic.written()) {
        digitalWrite(greenPIN, greenCharacteristic.value()[0]);
      }
      if (blueCharacteristic.written()) {
        digitalWrite(bluePIN, blueCharacteristic.value()[0]);
      }


 
      // Send data every 1 second
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= 1000) {
        previousMillis = currentMillis;
x=random(80,100);
    // Send an 8-bit unsigned integer:
uint8_t valueToSend = x;
greenCharacteristic.writeValue(valueToSend);

        Serial.println("Sent value: 55 to greenCharacteristic");
      }

      delay(10); // Give some breathing room

    }

    Serial.println("Disconnected from central");
  }
}
