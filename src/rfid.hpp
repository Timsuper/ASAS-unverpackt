#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h> //RFID

/*
SPI:
MISO - 50
MOSI - 51
SCK - 52

frei waelbar:
SS/SDA - 53
RST - 9
*/

#define SS_PIN 53 // Slave-Select / SDA (RFID)
#define RST_PIN 9 // Rest (RFID)
 
MFRC522 rfid(SS_PIN, RST_PIN); // Klasseninstanz

MFRC522::MIFARE_Key key;

class rfid_helper_class {
private:

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
} rfid_helper;