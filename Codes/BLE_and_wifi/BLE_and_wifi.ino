#include <WiFiNINA.h>
#include <ArduinoBLE.h>

// -------------------- Wi-Fi Credentials --------------------
char ssid[] = "ahmed";
char pass[] = "ahmedahmed";
char host[] = "webhook.site";
int port = 80;
String path = "/2e9b1986-e06e-46cb-b9fc-5eb07f0509ba";
WiFiClient client;

// -------------------- BLE Setup --------------------
BLEService LEDService("19b10000-e8f2-537e-4f6c-d104768a1214");
BLECharacteristic redCharacteristic("19b10001-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);
BLECharacteristic greenCharacteristic("19b10002-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);
BLECharacteristic blueCharacteristic("19b10003-e8f2-537e-4f6c-d104768a1214", BLERead | BLEWrite | BLENotify, 1);

// -------------------- Pins --------------------
const int redPIN = 13;
const int greenPIN = 12;
const int bluePIN = 11;

unsigned long previousMillis = 0;
int sensorValue = 0;
int mode = 0;
bool bleRunning = false;

void setup() {
  Serial.begin(9600);
  //while (!Serial);
  Serial.println("Type 1 for BLE mode or 2 for WiFi mode.");

  pinMode(redPIN, OUTPUT);
  pinMode(greenPIN, OUTPUT);
  pinMode(bluePIN, OUTPUT);

  digitalWrite(redPIN, LOW);
  digitalWrite(greenPIN, HIGH);
  digitalWrite(bluePIN, HIGH);
}

void loop() {
  if (Serial.available()) {
    String input = Serial.readStringUntil('\n');
    input.trim();

    if (input == "1") {
      mode = 1;
    } else if (input == "2") {
      mode = 2;
    } else if (input == "0") {
      mode = 0;
      if (bleRunning) {
        Serial.println("Interrupt received: Stopping BLE...");
        BLE.stopAdvertise();
        BLE.end();
        bleRunning = false;
      }
    } else {
      Serial.println("Invalid input. Type 1 for BLE, 2 for WiFi, or 0 to stop.");
    }
  }

  if (mode == 1 && !bleRunning) {
    runBLEMode();
    mode = 0;
    Serial.println("Type 1 for BLE mode or 2 for WiFi mode.");
  }

  if (mode == 2) {
    runWiFiMode();
    mode = 0;
    Serial.println("Type 1 for BLE mode or 2 for WiFi mode.");
  }
}

void runBLEMode() {
  Serial.println("You are in BLE mode");
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
  bleRunning = true;

  Serial.println("BLE advertising...");

  BLEDevice central;

  while (!central && mode != 0) {
    delay(200);
    central = BLE.central();
  }

  if (!central) {
    Serial.println("BLE mode interrupted before connection.");
    BLE.stopAdvertise();
    BLE.end();
    bleRunning = false;
    return;
  }

  Serial.print("Connected to central: ");
  Serial.println(central.address());

  while (central.connected() && mode != 0) {
    if (redCharacteristic.written()) {
      digitalWrite(redPIN, redCharacteristic.value()[0]);
    }
    if (greenCharacteristic.written()) {
      digitalWrite(greenPIN, greenCharacteristic.value()[0]);
    }
    if (blueCharacteristic.written()) {
      digitalWrite(bluePIN, blueCharacteristic.value()[0]);
    }

    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= 1000) {
      previousMillis = currentMillis;
      sensorValue = random(80, 100);
      greenCharacteristic.writeValue((uint8_t)sensorValue);
      Serial.print("Sent BLE value: ");
      Serial.println(sensorValue);
    }

    delay(10);

    // Check for interruption
    if (Serial.available()) {
      String input = Serial.readStringUntil('\n');
      input.trim();
      if (input == "0") {
        mode = 0;
        Serial.println("Interrupt received: Disconnecting BLE...");
        break;
      }
    }
  }

  Serial.println("BLE session ended.");
  BLE.stopAdvertise();
  BLE.end();
  bleRunning = false;
}

void runWiFiMode() {
  Serial.println("You are in WiFi mode");

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");

  String csv = "ECG data\n";
  for (int i = 0; i < 900; i++) {
    int value = random(80, 100);
    csv += "," + String(value);
  }

  if (client.connect(host, port)) {
    client.print("POST " + path + " HTTP/1.1\r\n");
    client.print("Host: " + String(host) + "\r\n");
    client.println("Content-Type: text/csv");
    client.print("Content-Length: ");
    client.println(csv.length());
    client.println("Connection: close");
    client.println();
    client.print(csv);

    Serial.println("CSV sent over WiFi");

    // Wait for server response
    unsigned long timeout = millis();
    while (client.connected() && millis() - timeout < 5000) {
      if (client.available()) {
        String line = client.readStringUntil('\n');
      //  Serial.println(line);
      }
    }

    delay(1000); // Give time for data to flush out
  } else {
    Serial.println("Connection to webhook.site failed.");
  }

  client.stop();
  WiFi.disconnect();
  WiFi.end();
}

