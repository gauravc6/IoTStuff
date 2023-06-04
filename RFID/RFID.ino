#include <M5Core2.h>
#include <SPI.h>
#include <MFRC522.h>

// VCC -> 3.3V, RST -> G27, ground -> ground, MISO -> MISO, MOSI -> MOSI, SCK -> SCK, SDA -> G19

#define SS_PIN 19
#define RST_PIN 27

String cardData = "";

MFRC522 rfid(SS_PIN, RST_PIN);

void setup() {
  M5.begin();
  Serial.begin(115200);
  SPI.begin();
  rfid.PCD_Init();

  Serial.println("Tap a RFID card/tag");
}

void loop() {
  M5.Lcd.clear();
  if (rfid.PICC_IsNewCardPresent()) {
    if (rfid.PICC_ReadCardSerial()) {
      MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
      Serial.print("RFID tag type: ");
      Serial.println(rfid.PICC_GetTypeName(piccType));
      Serial.print("UID: ");
      for (int i = 0; i < rfid.uid.size; ++i) {
        // Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        // Serial.print(rfid.uid.uidByte[i], HEX);
        cardData.concat(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
        cardData.concat(String(rfid.uid.uidByte[i], HEX));
        Serial.print(rfid.uid.uidByte[i], HEX);
        Serial.print(" ");
      }
      Serial.println();
      cardData.toUpperCase();
      M5.Lcd.setTextSize(2);
      M5.Lcd.setCursor(10, 10);
      if (cardData.substring(1) == "D1 24 0A 24") {
        Serial.println("Access Granted!");
        M5.Lcd.setTextColor(GREEN);
        M5.Lcd.print("Access Granted!");
      } else {
        Serial.println("Access Denied!");
        M5.Lcd.setTextColor(RED);
        M5.Lcd.print("Access Denied!");
      }
      delay(1000);
      cardData = "";
      rfid.PICC_HaltA();       // Halt PICC
      rfid.PCD_StopCrypto1();  // Stop encryption on PCD
    }
  }
}