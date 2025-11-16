#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

Adafruit_SH1106G display(128, 64, &Wire, -1); // Width, Height, Wire, Reset pin (-1 = no reset)

void setup() {
  Serial.begin(115200);

  if (!display.begin(0x3C)) {
    Serial.println(F("SH1106 not found"));
    while (1);
  }

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SH110X_WHITE);

  
  
}
int x=0;
void loop() {
   display.clearDisplay();
     display.setCursor(5, 25);
display.print("Z =");
    display.println(x);
    display.display();
x++;
  delay(100);
}
