#include <WiFi.h>
#include <HTTPClient.h>
#include <M5Core2.h>
#include <DHT.h>

#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

float temp = 0, hum = 0;

const char* ssid = "some_ssid";
const char* password = "some_psw";

void initWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  Serial.println(WiFi.localIP());
}

void setup() {
  M5.begin();
  Serial.begin(115200);
  dht.begin();
  initWifi();
}

void loop() {
  HTTPClient http;
  ReadDHT();
  http.begin("thing_speak_api_url" + String(temp) +  "&field2=" + String(hum));
  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.println("Error while making http request.");
  }
  http.end();
  delay(20000);
}

void ReadDHT() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read error.");
    M5.Lcd.clear();
    M5.Lcd.printf("DHT read error.");
    return;
  }
  M5.Lcd.clear();
  Serial.printf("Temperature: %.1f Humidity: %.1f\n", temp, hum);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("Temperature: %.1f Humidity: %.1f", temp, hum);
}