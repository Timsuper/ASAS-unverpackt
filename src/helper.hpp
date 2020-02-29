#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <loadcell.hpp>
#include <rfid.hpp>
#include <display.hpp>

class rfid_test_class {
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
  void print_nuid(byte *buffer, byte bufferSize) {
    Serial.println(F("The NUID tag is:"));
    Serial.print(F("In hex: "));
    printHex(buffer, bufferSize);
    Serial.println();
    Serial.print(F("In dec: "));
    printDec(buffer, bufferSize);
    Serial.println();
  }

  bool compare_rfid_bytes(byte card1[4], byte card2[4]){
    for (int i=0; i<=3; i++) {
      if (card1[i] != card2[i]) return false;
    }
    return true;
  }
} rfid_test;