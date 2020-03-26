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
Kontaktschalter - D18 INPUT PULLUP
Schlosspin - D7 OUTPUT

Mögliche Optimierungen:
- millis() Code gegen overflows umschreiben
*/

String product_name = "Test";
const float price_per_g = 0.1;
const int price_per_X_g = 100;

const int doorlock_pin = 7;
const int contactpin = 18; // erlaubt: 2, 3, 18, 19, 20, 21

bool contactpin_closed = false;

int debug_milis_timer = 0;

void open_lock(int time = 100) {
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
  pinMode(doorlock_pin, OUTPUT);
  digitalWrite(doorlock_pin, false);

  display.init();
  rfid_helper.init();
  loadcell.init();

  debug_milis_timer = millis();
  Serial.println("setup() abgeschlossen");
}

void loop() {
  /*
  int new_debug_millis_timer = millis();
  const int delay_time = 3000;
  if (new_debug_millis_timer-debug_milis_timer < 1*delay_time) {
    display.mode_normal();
    Serial.println("display_normal");
  } else if (new_debug_millis_timer-debug_milis_timer < 2*delay_time) {
    display.mode_opened_case();
    Serial.println("display_opened_case");
  } else if (new_debug_millis_timer-debug_milis_timer < 3*delay_time) {
    display.mode_closed_case();
    Serial.println("display_closed_case");
  } else if (new_debug_millis_timer-debug_milis_timer < 4*delay_time) {
    display.mode_empty_container();
    Serial.println("display_empty_container");
  } else if (new_debug_millis_timer-debug_milis_timer < 5*delay_time) {
    display.mode_error();
    Serial.println("display_error");
  } else if (new_debug_millis_timer-debug_milis_timer < 6*delay_time) {
    display.mode_error("Errorcode xyz");
    Serial.println("display_error_xyz");
  } else {
    debug_milis_timer = new_debug_millis_timer;
  }
  */
  
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

  // Interrupts noch vor der Erkennung einer RFID Karte ändern
  // da das attachen von Interrupts Zeitintensiv ist
  detachInterrupt(digitalPinToInterrupt(contactpin));
  attachInterrupt(digitalPinToInterrupt(contactpin), ISR_contactpin_rising, RISING);
  
  //wenn Karte fest hinterlegt
  if (rfid_helper.compare_rfid_bytes(rfid_helper.nuidPICC, rfid_helper.vaild_card_1)) {
    // Kundennummer formatieren
    String kundennummer;
    kundennummer = String(rfid_helper.nuidPICC[0], HEX) + " " + String(rfid_helper.nuidPICC[1], HEX) + " " + String(rfid_helper.nuidPICC[2], HEX) + " " + String(rfid_helper.nuidPICC[3], HEX);

    display.mode_opened_case(kundennummer);

    // Gewicht ermitteln in loadcell (vor öffnen -> einmal!)
    loadcell.tare();

    contactpin_closed = false;
    // Schloss öffnen
    open_lock();

    // Schloss zu?
    int messure_time = millis();
    double current_weight = 0;
    float current_price = 0;

    while (contactpin_closed == false) {
      // Sicherstellen, dass nur jede Sekunde gemessen wird
      // Der Displaybuffer muss bedient werden!
      if (int new_messure_time = millis()-messure_time >= 1000) {
        // Gewicht kontinuierlich ermitteln
        current_weight = abs(loadcell.get_units());
        // Preisberechnung kontinuierlich
        current_price = abs(current_weight * price_per_g);
        messure_time = new_messure_time;
      }
      // Display kontinutierlich
      display.mode_opened_case(kundennummer, current_price, current_weight);
    }

    //detachInterrupt(digitalPinToInterrupt(contactpin));
    //attachInterrupt(digitalPinToInterrupt(contactpin), ISR_contactpin_falling, FALLING);

    // Gewicht neu ermitteln in loadcell
    double final_weight = abs(loadcell.get_units());

    // Preisberechnung final
    float final_price = abs(final_weight * price_per_g);

    // Display Abrechnung
    display.mode_closed_case(kundennummer, final_price, final_weight);

    delay(5000);

    // Gewicht 0 oder niedrig? -> Box leer anzeigen
    if (final_weight <= 100 && final_weight >= 0) {
        display.mode_empty_container();
        while (true) {}
    }

    delay(5000);
  } else {
    display.mode_unknown_card();
    delay(5000);
  }

  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD, notwending um neue Karte lesen zu koennen
  rfid.PCD_StopCrypto1();
}