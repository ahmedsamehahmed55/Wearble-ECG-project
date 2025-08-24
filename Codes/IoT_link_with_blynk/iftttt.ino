#include <ArduinoBLE.h>

// Create a BLE Service for Heart Rate (standard UUID)
BLEService heartRateService("180D");

// Create a characteristic for heart rate measurement (standard UUID)
BLECharacteristic heartRateChar("2A37",  // UUID for Heart Rate Measurement
                                BLERead | BLENotify);

// Set a constant heart rate value (72 bpm)
const byte heartRateValue[2] = {0x00, 72}; // Flags=0x00, HR=72 bpm

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  BLE.setLocalName("HRM_Nano33");
  BLE.setAdvertisedService(heartRateService);

  // Add the characteristic to the service
  heartRateService.addCharacteristic(heartRateChar);

  // Add the service to the BLE stack
  BLE.addService(heartRateService);

  // Set the initial value
  heartRateChar.writeValue(heartRateValue, sizeof(heartRateValue));

  // Start advertising
  BLE.advertise();

  Serial.println("BLE Heart Rate Sensor is now advertising...");
}

void loop() {
  // Poll for BLE events
  BLE.poll();
}
