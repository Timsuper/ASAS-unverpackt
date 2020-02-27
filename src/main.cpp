#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <helper.hpp>

void setup() {
  Serial.begin(9600);
  SPI.begin();
  /*
  rfid.PCD_Init();

  for (byte i = 0; i <= 3; i++) {
    key.keyByte[i] = 0xFF;
  }

  Serial.println(F("This code scan the MIFARE Classic NUID."));
  Serial.print(F("Using the following key:"));
  printHex(key.keyByte, MFRC522::MF_KEY_SIZE);

  Serial.println("");
  Serial.print("rfid.uid.size= ");
  Serial.println(rfid.uid.size);
  */
  loadcell_test.loadcell_test_init();
}

void loop() {
  while(!Serial.available()) {}
  loadcell_test.loadcell_case_handler_test();
  /*
  if (!rfid.PICC_IsNewCardPresent())
  return;

  if (!rfid.PICC_ReadCardSerial())
  return;

  Serial.print(F("PICC type: "));
  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);
  Serial.println(rfid.PICC_GetTypeName(piccType));

  // Check is the PICC of Classic MIFARE type
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    Serial.println(F("Fehler: Your tag is not of type MIFARE Classic."));
    return;
  }

  if (rfid.uid.uidByte[0] != nuidPICC[0] || 
    rfid.uid.uidByte[1] != nuidPICC[1] || 
    rfid.uid.uidByte[2] != nuidPICC[2] || 
    rfid.uid.uidByte[3] != nuidPICC[3] ) {
    // Store new NUID into nuidPICC array
    for (byte i = 0; i <= 3; i++) {
      nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println(F("A new card has been detected."));
    print_nuid(rfid.uid.uidByte, rfid.uid.size);
    Serial.print(" size: ");
    Serial.println(rfid.uid.size);
  }
  else Serial.println(F("Card read previously."));

  //wenn Karte fest hinterlegt
  if (compare_rfid_bytes(nuidPICC, vaild_card_1)) {
    Serial.println("check bestanden");
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD, notwending um neue Karte lesen zu koennen
  rfid.PCD_StopCrypto1();
  */
}