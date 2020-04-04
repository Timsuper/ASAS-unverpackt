#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h> // RFID
 
MFRC522 rfid(RFID_SS_PIN, RFID_RST_PIN); // Klasseninstanz; benutzt HW SPI

MFRC522::MIFARE_Key key;

class rfid_helper_class {
private:
  void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], HEX);
    }
  }

  void printDec(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
      Serial.print(buffer[i] < 0x10 ? " 0" : " ");
      Serial.print(buffer[i], DEC);
    }
  }
public:
    byte nuidPICC[4];
    byte vaild_card_1[4] = {0xF9, 0x06, 0xCB, 0x7E};

    void init() {
        rfid.PCD_Init();

        for (byte i = 0; i <= 3; i++) {
            key.keyByte[i] = 0xFF;
        }

        Serial.println("RFID init() abgeschlossen");
    }

    bool compare_rfid_bytes(byte card1[4], byte card2[4]){
        for (int i=0; i<=3; i++) {
            if (card1[i] != card2[i]) return false;
        }
        return true;
    }

    void print_nuid(byte *buffer, byte bufferSize) {
        Serial.println(F("The NUID tag is:"));
        Serial.print(F("In hex: "));
        printHex(buffer, bufferSize);
        Serial.println();
        Serial.print(F("In dec: "));
        printDec(buffer, bufferSize);
        Serial.println();
    }
} rfid_helper;