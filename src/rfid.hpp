#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <MFRC522.h> //RFID

#define SS_PIN 53 // Slave-Select (RFID)
#define RST_PIN 9 // Rest (RFID)
 
MFRC522 rfid(SS_PIN, RST_PIN); // Klasseninstanz

MFRC522::MIFARE_Key key;

class rfid_helper_class {
private:

public:
    byte nuidPICC[4];
    byte vaild_card_1[4] = {0xD4, 0xFD, 0x3B, 0x1E};

    void init() {
        rfid.PCD_Init();

        for (byte i = 0; i <= 3; i++) {
            key.keyByte[i] = 0xFF;
        }
    }

    bool compare_rfid_bytes(byte card1[4], byte card2[4]){
        for (int i=0; i<=3; i++) {
            if (card1[i] != card2[i]) return false;
        }
        return true;
    }
} rfid_helper;