#include <M5Core2.h>
#include <TinyGPSPlus.h>

// VCC -> 5V, RX -> TX(G14), TX -> RX(G13), ground -> ground
TinyGPSPlus gps;

void setup() {
  M5.begin();
  Serial.begin(115200);
  Serial2.begin(9600);
  delay(5000);
}

void loop() {
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      displayLocation();
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    M5.Lcd.printf("No GPS detected! - check wiring");
    while (true)
      ;
  }
}

void displayLocation() {
  // Serial.print(F("Coordinate: "));
  // M5.Lcd.print("Coordinate: ");
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(2);
  if (gps.location.isValid()) {
    // Serial.print("Lat: ");
    // Serial.print(gps.location.lat(), 6);
    // Serial.print(F(", Long: "));
    // Serial.print(gps.location.lng(), 6);
    M5.Lcd.printf("%.6f", gps.location.lat(), 6);
    M5.Lcd.setCursor(10, 50);
    M5.Lcd.printf("%.6f", gps.location.lng(), 6);
  } else {
    M5.Lcd.print("Invalid location!");
  }
  Serial.println();
}