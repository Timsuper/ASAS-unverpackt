#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h> //RFID

#define SS_PIN 53 // Slave-Select (RFID)
#define RST_PIN 5 // Rest (RFID)
 
MFRC522 rfid(SS_PIN, RST_PIN); // Klasseninstanz

MFRC522::MIFARE_Key key;

byte nuidPICC[4];

byte vaild_card_1[] = {0xD4, 0xFD, 0x3B, 0x1E};
//byte vaild_card_2[] = {0x00, 0x00, 0x00, 0x00};