#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <helper.hpp>

String product_name = "Test";
float price_per_g = 0.25;
int price_per_X_g = 100;

const int schloss_pin = 4;
const int kontaktschalter_pin = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin();

  //Schloss Pin
  pinMode(schloss_pin, OUTPUT);
  digitalWrite(schloss_pin, false);

  display.init();
  rfid_helper.init();
  loadcell.init();
}

void loop() {
  display.display_mode_normal(product_name, price_per_X_g, price_per_g*price_per_X_g);

  if (!rfid.PICC_IsNewCardPresent())
  return;

  if (!rfid.PICC_ReadCardSerial())
  return;

  MFRC522::PICC_Type piccType = rfid.PICC_GetType(rfid.uid.sak);

  // Prüfen ob Karte gelesen werden kann
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI &&  
    piccType != MFRC522::PICC_TYPE_MIFARE_1K &&
    piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }

  if (rfid.uid.uidByte[0] != rfid_helper.nuidPICC[0] || 
    rfid.uid.uidByte[1] != rfid_helper.nuidPICC[1] || 
    rfid.uid.uidByte[2] != rfid_helper.nuidPICC[2] || 
    rfid.uid.uidByte[3] != rfid_helper.nuidPICC[3] ) {
    // Store new NUID into nuidPICC array
    for (byte i = 0; i <= 3; i++) {
      rfid_helper.nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println(F("A new card has been detected."));
    rfid_test.print_nuid(rfid.uid.uidByte, rfid.uid.size);
  }
  else Serial.println(F("Card read previously."));

  //wenn Karte fest hinterlegt
  if (rfid_helper.compare_rfid_bytes(rfid_helper.nuidPICC, rfid_helper.vaild_card_1)) {
    // Kundennummer formatieren
    String kundennummer;
    kundennummer = String(rfid_helper.nuidPICC[0]) + " " + String(rfid_helper.nuidPICC[1]) + " " + String(rfid_helper.nuidPICC[2]) + " " + String(rfid_helper.nuidPICC[3]);

    // Gewicht ermitteln in loadcell (vor öffnen -> einmal!)
    double first_weight = loadcell.get_units();

    // Schloss öffnen
    digitalWrite(schloss_pin, true);
    delay(500);
    digitalWrite(schloss_pin, false);

    // Gewicht kontinuierlich ermitteln
    double current_weight = loadcell.get_units();

    // Preisberechnung kontinuierlich
    float current_price = current_weight * price_per_g;

    // Display kontinutierlich
    display.display_mode_opened_case(kundennummer, 0);

    // Schloss zu?

    // Gewicht neu ermitteln in loadcell

    // Preisberechnung final

    // Display Abrechnung
    display.display_mode_closed_case(kundennummer, 0, 0);

    // Gewicht 0 oder niedrig? -> Box leer anzeigen

    delay(5000);
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD, notwending um neue Karte lesen zu koennen
  rfid.PCD_StopCrypto1();
}