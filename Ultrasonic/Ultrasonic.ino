#include <M5Core2.h>

#define soundSpeed 0.034 // Speed of sound in cm/µs
// Vcc -> 5V, Trig -> G25, Echo -> G26, ground -> ground

const int trigPin = 25;
const int echoPin = 26;

long duration;
float distanceCm;

void setup() {
  M5.begin();
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
}

void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);
  distanceCm = duration * soundSpeed / 2;
  Serial.printf("Distance in (cm) %f\n", distanceCm);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(7);
  M5.Lcd.printf("%.1f", distanceCm);
  delay(500);
}
