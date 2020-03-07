#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <helper.hpp>

/*
Pinout Arduino Mega

SPI:
MISO - 50
MOSI - 51
SCK - 52

frei waelbar:
SS/SDA - 53

Eigene:
Kontaktschalter - D6 INPUT PULLUP
Schlosspin - D7 OUTPUT
*/

String product_name = "Test";
const float price_per_g = 0.25;
const int price_per_X_g = 100;

const int doorlock_pin = 7;
const int contactpin = 18; // erlaubt: 2, 3, 18, 19, 20, 21

bool contactpin_closed = false;

int debug_milis_timer = 0;

void open_lock(int time = 200) {
  // Schloss für time ms öffnen
  digitalWrite(doorlock_pin, true);
  delay(time);
  digitalWrite(doorlock_pin, false);
}

void ISR_contactpin_rising() {
  contactpin_closed = true;
}

void ISR_contactpin_falling() {
  contactpin_closed = false;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  // Schloss Pin
  //pinMode(doorlock_pin, OUTPUT);
  //digitalWrite(doorlock_pin, false);

  // Kontaktschalter Pin
  //pinMode(contactpin, INPUT_PULLUP);

  display.init();
  //rfid_helper.init();
  //loadcell.init();

  debug_milis_timer = millis();
  Serial.println("setup() abgeschlossen");
}

void loop() {
  int new_debug_millis_timer = millis();
  const int delay_time = 3000;
  if (new_debug_millis_timer-debug_milis_timer < 1*delay_time) {
    display.mode_normal();
  } else if (new_debug_millis_timer-debug_milis_timer < 2*delay_time) {
    display.mode_opened_case();
  } else if (new_debug_millis_timer-debug_milis_timer < 3*delay_time) {
    display.mode_closed_case();
  } else if (new_debug_millis_timer-debug_milis_timer < 4*delay_time) {
    display.mode_empty_container();
  } else if (new_debug_millis_timer-debug_milis_timer < 5*delay_time) {
    display.mode_error();
  } else if (new_debug_millis_timer-debug_milis_timer < 6*delay_time) {
    display.mode_error("Errorcode xyz");
  } else {
    debug_milis_timer = new_debug_millis_timer;
  }

  return;
  
  display.mode_normal(product_name, price_per_X_g, price_per_g*price_per_X_g);

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
    open_lock();

    if (contactpin_closed == false) {
        display.mode_error();
        while (true) {}
    }
    
    detachInterrupt(digitalPinToInterrupt(contactpin));
    attachInterrupt(digitalPinToInterrupt(contactpin), ISR_contactpin_rising, RISING);

    // Schloss zu?
    while (contactpin_closed == false) {
      // Gewicht kontinuierlich ermitteln
      double current_weight = loadcell.get_units();

      // Preisberechnung kontinuierlich
      float current_price = current_weight * price_per_g;

      // Display kontinutierlich
      display.mode_opened_case(kundennummer, current_price);

      delay(1000);
    }

    detachInterrupt(digitalPinToInterrupt(contactpin));
    attachInterrupt(digitalPinToInterrupt(contactpin), ISR_contactpin_falling, FALLING);

    // Gewicht neu ermitteln in loadcell
    double final_weight = loadcell.get_units();

    // Preisberechnung final
    float final_price = final_weight * price_per_g;

    // Display Abrechnung
    display.mode_closed_case(kundennummer, final_price, final_weight);

    // Gewicht 0 oder niedrig? -> Box leer anzeigen
    if (final_weight <= 100) {
        display.mode_empty_container();
        while (true) {}
    }

    delay(5000);
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD, notwending um neue Karte lesen zu koennen
  rfid.PCD_StopCrypto1();
}