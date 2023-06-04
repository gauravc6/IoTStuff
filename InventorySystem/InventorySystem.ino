#include <M5Core2.h>
#include <SPI.h>
#include <MFRC522.h>
#include <WiFi.h>
#include <HTTPClient.h>

#define SOUND_SPEED 0.034  // Speed of sound in cm/µs
#define BOX_LENGTH 10

// Ultrasonic config
// Vcc -> 5V, Trig -> G25, Echo -> G26, ground -> ground
#define TRIG_PIN 25
#define ECHO_PIN 26

// RFID config
// VCC -> 3.3V, RST -> G27, ground -> ground, MISO -> MISO, MOSI -> MOSI, SCK -> SCK, SDA -> G19
#define SS_PIN 19
#define RST_PIN 27

int authorization[2] = { 0 };
const char* ssid = "some_ssid";
const char* password = "some_psw";
MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  M5.begin();
  Serial.begin(115200);
  initWifi();
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  rfid.PCD_Init();

  Serial.println("Tap an Item RFID card/tag");
}

void loop() {
  // add prompt for tapping
  int itemsPicked = getNumberOfPickedItems();
  updateAuthorizedItems();
  validateItemQuantity(itemsPicked);
  createHttpRequest(itemsPicked);
}

void validateItemQuantity(int nItems) {
  M5.Lcd.setTextSize(2);
  M5.Lcd.setCursor(10, 10);
  M5.Lcd.clearDisplay();
  if (nItems == authorization[0] + authorization[1]) {
    Serial.print("Item Quantity Consistent");
    M5.Lcd.setTextColor(GREEN);
    M5.Lcd.print("Item Quantity Consistent");
  } else {
    Serial.print("Item Quantity Mismatch");
    M5.Lcd.setTextColor(RED);
    M5.Lcd.print("Item Quantity Mismatch");
  }
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

void createHttpRequest(int items) {
  HTTPClient http;
  Serial.println();
  http.begin("thing_speak_api_url" + String(items) + "&field2=" + String(authorization[1]) + "&field3=" + String(authorization[0]));
  int httpCode = http.GET();
  if (httpCode <= 0) {
    Serial.println("Error while making http request.");
  }
  http.end();
  delay(5000);
}

void updateAuthorizedItems() {
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      String cardData = "";
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      for (int i = 0; i < rfid.uid.size; ++i) {
        cardData.concat(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        cardData.concat(String(rfid.uid.uidByte[i], HEX));
      }
      cardData.toUpperCase();

      authorization[cardData.substring(1) == "D1 24 0A 24"]++;
      Serial.printf("Number of Authorized items: %d", authorization[1]);
      Serial.println();
      delay(1000);
      cardData = "";
      rfid.PICC_HaltA();       // Halt PICC
      rfid.PCD_StopCrypto1();  // Stop encryption on PCD
    }
  }
}

int getNumberOfPickedItems() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance = duration * SOUND_SPEED / 2;
  Serial.printf("Distance in (cm) %f\n", distance);
  int pickedUpItems = distance / BOX_LENGTH;
  return pickedUpItems;
}