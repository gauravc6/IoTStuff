#include <M5Core2.h>
#include <TinyGPSPlus.h>
#include <DHT.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define TEMP_LOW 30
#define TEMP_HIGH 35
#define HUM_LOW 62
#define HUM_HIGH 70

// GPS Module config
// VCC -> 5V, RX -> TX(G14), TX -> RX(G13), ground -> ground
TinyGPSPlus gps;

// LDR Sensor config
// VCC -> G26(HIGH), ground -> ground, DO -> G27
#define LDRPower 26
#define LDRSensor 27

// DHT Sensor config
// + -> 5V, out -> G19, - -> ground
#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "some_ssid";
const char* password = "some_psw";

float temp = 0, hum = 0, lat = 0.0, lng = 0.0;
int lightPresence = 0;

void setup() {
  M5.begin();
  setupLDR();
  dht.begin();
  initWifi();
  Serial.begin(115200);
  Serial2.begin(9600);
}

void loop() {
  readDHT();
  updateLightData();
  updateGpsCoords();
  int status = getStatus();
  updateDisplay(status);
  createHttpRequest(status);
}

void setupLDR() {
  pinMode(LDRSensor, INPUT);
  pinMode(LDRPower, OUTPUT);
  digitalWrite(LDRPower, HIGH);
}

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

bool getStatus() {
  return temp <= TEMP_HIGH && temp >= TEMP_LOW && hum <= HUM_HIGH && hum >= HUM_LOW && lightPresence == 0;
}

void updateDisplay(int status) {
  M5.Lcd.clear();
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  if (status) {
    M5.lcd.print("ALL OK!");
    M5.Lcd.fillScreen(GREEN);
  } else {
    M5.Lcd.fillScreen(RED);
    M5.lcd.print("NOT OK!");
  }
}

void createHttpRequest(bool status) {
  HTTPClient http;
  Serial.println();
  http.begin("thing_speak_api_url" + String(temp) + "&field2=" + String(hum) + "&field3=" + String(lightPresence) + "&field4=" + String(lat) + "&field5=" + String(lng) + "&field6=" + String(status));
  int httpCode = http.GET();
  if (httpCode <= 0) {
    Serial.println("Error while making http request.");
  }
  http.end();
  delay(15000);
}

void readDHT() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read error.");
    return;
  }
  Serial.printf("Temperature: %.1f Humidity: %.1f\n", temp, hum);
}

void updateLightData() {
  lightPresence = digitalRead(LDRSensor);
}

void updateGpsCoords() {
  while (Serial2.available() > 0) {
    if (gps.encode(Serial2.read())) {
      getLocation();
    }
  }
  if (millis() > 5000 && gps.charsProcessed() < 10) {
    M5.Lcd.printf("No GPS detected! - check wiring");
  }
}

void getLocation() {
  if (gps.location.isValid()) {
    lat = gps.location.lat();
    lng = gps.location.lng();
  } else {
    M5.Lcd.print("Invalid location!");
  }
}