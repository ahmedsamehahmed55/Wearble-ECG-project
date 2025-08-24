#include <ArduinoBLE.h>

// UUIDs for Heart Rate Service and Measurement Characteristic
BLEService heartRateService("180D");
BLECharacteristic heartRateChar("2A37",  // Heart Rate Measurement
                                BLERead | BLENotify, 
                                2); // 2 bytes (flags + heart rate)

int heartRate = 70; // Simulated heart rate value

void setup() {
  Serial.begin(9600);
  while (!Serial);

  if (!BLE.begin()) {
    Serial.println("Starting BLE failed!");
    while (1);
  }

  // Set device name and local name
  BLE.setDeviceName("Nano33IoT");
  BLE.setLocalName("HeartRateMonitor");
  BLE.setAdvertisedService(heartRateService);

  // Add characteristics to service
  heartRateService.addCharacteristic(heartRateChar);
  BLE.addService(heartRateService);

  // Set initial value for characteristic
  uint8_t hrData[2] = {0x00, heartRate};  // 0x00 = no flags
  heartRateChar.writeValue(hrData, 2);

  // Start advertising
  BLE.advertise();
  Serial.println("BLE Heart Rate Monitor is now advertising...");
}

void loop() {
  BLEDevice central = BLE.central();

  if (central) {
    Serial.print("Connected to: ");
    Serial.println(central.address());

    while (central.connected()) {
      delay(1000); // 1 second between heartbeats

      heartRate = random(60, 100); // Simulate heart rate

      uint8_t hrData[2] = {0x00, (uint8_t)heartRate}; // Flags, Heart rate
      heartRateChar.writeValue(hrData, 2); // Notify app

      Serial.print("Heart Rate: ");
      Serial.println(heartRate);
    }

    Serial.println("Disconnected.");
  }
}
