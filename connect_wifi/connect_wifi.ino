#include <WiFi.h>
#include <M5Core2.h>

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
  initWifi();
}

void loop() {
  Serial.printf("RSSI: %d\n", WiFi.RSSI());
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.setTextSize(7);
  M5.Lcd.printf("%d", WiFi.RSSI());
  delay(500);
}
