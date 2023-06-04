#include <M5Core2.h>
#include <DHT.h>

// + -> 5V, out -> G19, - -> ground
#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temp = 0, hum = 0;

void setup() {
  M5.begin();
  Serial.begin(115200);
  dht.begin();
}

void loop() {
  ReadDHT();
}


void ReadDHT() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read error.");
    M5.Lcd.clear();
    M5.Lcd.printf("DHT read error.");
    delay(2000);
    return;
  }
  M5.Lcd.clear();
  Serial.printf("Temperature: %.1f Humidity: %.1f\n", temp, hum);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("Temperature: %.1f Humidity: %.1f", temp, hum);
  delay(2000);
}