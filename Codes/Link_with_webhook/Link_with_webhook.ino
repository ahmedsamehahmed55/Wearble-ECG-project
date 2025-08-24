#include <WiFiNINA.h>

char ssid[] = "ahmed";
char pass[] = "ahmedahmed";

char host[] = "webhook.site";  // Don't include https://
int port = 80;
String path = "/2e9b1986-e06e-46cb-b9fc-5eb07f0509ba";  // Only the path part after the domain
WiFiClient client;

String createCSV() {
  String csv = "time,temperature,humidity\n";
  for(int i=0;i<900;i++){
int a=random(80,100);
csv= csv+","+a;
  }
  return csv;
}

void setup() {
  Serial.begin(9600);
  //while (!Serial);

  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected!");
}

void loop() {
  if (client.connect(host, port)) {
    Serial.println("Connected to webhook.site");

    String csvData = createCSV();

    client.print("POST " + path + " HTTP/1.1\r\n");
    client.print("Host: " + String(host) + "\r\n");
    client.println("Content-Type: text/csv");
    client.print("Content-Length: ");
    client.println(csvData.length());
    client.println("Connection: close");
    client.println(); // End of headers
    client.print(csvData); // Send the actual CSV data

    Serial.println("CSV sent:");
    Serial.println(csvData);
    
  } else {
    Serial.println("Connection to webhook.site failed.");
  }

  client.stop();  // Close the connection
  delay(5000);   // Wait for 10 seconds
}
