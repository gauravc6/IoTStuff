#include <M5Core2.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHTPIN 19
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

int LDRSensor = 27;

float temp = 0, hum = 0;

const char* SSID = "some_ssid";
const char* PASSWORD = "some_psw";

const char* mqtt_broker = "192.168.1.13";
const char* topic = "sensor";
const char* mqtt_username = "user1";
const char* mqtt_password = "root";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  pinMode(LDRSensor, INPUT);
  M5.begin();
  Serial.begin(115200);
  dht.begin();
  initWiFi();
  connectMQTT();
}

void loop() {
  M5.Lcd.clear();
  client.loop();
  readSensors();
  delay(5000);
}

void readSensors() {
  temp = dht.readTemperature();
  hum = dht.readHumidity();
  int data = digitalRead(LDRSensor);
  if (isnan(temp) || isnan(hum)) {
    Serial.println("DHT read error.");
    M5.Lcd.clear();
    M5.Lcd.printf("DHT read error.");
    delay(2000);
    return;
  }
  Serial.printf("Temperature: %.1f Humidity: %.1f Light: %d\n", temp, hum, data);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.printf("Temperature: %.1f Humidity: %.1f Light: %d", temp, hum, data);

  StaticJsonDocument<256> doc;
  doc["temperature"] = round(temp);
  doc["humidity"] = round(hum);
  doc["light"] = data;

  char out[128];
  int x = serializeJson(doc, out);  
  // Serial.print("Bytes: ");
  // Serial.println(x, DEC);
  client.publish(topic, out);
  delay(2000);
}

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("Connected to WiFi: ");
  Serial.print(WiFi.localIP());
}

void connectMQTT() {
  client.setServer(mqtt_broker, mqtt_port);
  client.setCallback(callback);
  while (!client.connected()) {
    String client_id = "M5stack-";
    client_id += (String)WiFi.macAddress();
    Serial.println("MQTT Client ID: " + client_id);
    if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected with MQTT Broker");
    } else {
      Serial.println("Failed state: ");
      Serial.print(client.state());
      delay(2000);
    }
  }
  client.publish(topic, "\"temp\": .");
  // client.subscribe(topic);
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Recieved message for the topic: ");
  Serial.println(topic);
  Serial.print("Message: ");
  for (int i = 0; i < length; ++i) {
    Serial.print((char)payload[i]);
  }
  Serial.println("\n------------------");
}