#include <M5Core2.h>

// VCC -> 5V, ground -> ground, DO -> G27
int LDRSensor = 27;

void setup() {
  pinMode(LDRSensor, INPUT);
  Serial.begin(115200);
  M5.begin();
}

void loop() {
  int sensorData = digitalRead(LDRSensor);
  Serial.printf("Sensor value: %d\n", sensorData);
  if (sensorData == 0) {
    Serial.println("Light detected!");
    M5.Lcd.fillScreen(BLUE);
  } else {
    Serial.println("No light detected!");
    M5.Lcd.fillScreen(BLACK);
  }
  delay(1000);
}
