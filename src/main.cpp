#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <rfid.hpp>
#include <display.hpp>
#include <loadcell.hpp>
#include <ultrasound.hpp>

/*
Pinout Arduino Mega

SPI:
MISO - 50
MOSI - 51
SCK - 52

frei waelbar:
SS/SDA - 53

Eigene:
Kontaktschalter - D18 INPUT
Schlosspin - D7 OUTPUT

Mögliche Optimierungen:
- millis() Code gegen overflows umschreiben
*/

#define PIN_DOORLOCK 7
#define PIN_CONTACT 18

#define USE_ULTRASOUND false
#define USE_LOADCELL true

const String product_name = "Test";

const float price_per_g = 0.1;
const int price_per_X_g = 100;
const float weight_per_1cm_height_g = 40;

volatile bool contactpin_status = true;

int debug_milis_timer = 0;

void open_lock(int time = 100) {
  // Schloss für time ms öffnen
  digitalWrite(PIN_DOORLOCK, true);
  delay(time);
  digitalWrite(PIN_DOORLOCK, false);
}

void ISR_contactpin_rising() {
  contactpin_status = true;
}

void ISR_contactpin_falling() {
  contactpin_status = false;
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  // Schloss Pin
  pinMode(PIN_DOORLOCK, OUTPUT);
  digitalWrite(PIN_DOORLOCK, false);

  display.init();
  rfid_helper.init();

  if (USE_LOADCELL) {
    loadcell.init();
  }
  if (USE_ULTRASOUND) {
    ultrasound.init();
  }

  debug_milis_timer = millis();
  Serial.println("setup() abgeschlossen");
}

void loop() {
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
    // schauen ob eine neue Karte erkannt wurde
    for (byte i = 0; i <= 3; i++) {
      rfid_helper.nuidPICC[i] = rfid.uid.uidByte[i];
    }
    Serial.println(F("Neue Karte erkannt"));
    rfid_helper.print_nuid(rfid.uid.uidByte, rfid.uid.size);
  }
  else Serial.println(F("Vorherige Karte erkannt"));
  
  // wenn Karte fest hinterlegt
  // kein Anlern-System implementiert
  if (rfid_helper.compare_rfid_bytes(rfid_helper.nuidPICC, rfid_helper.vaild_card_1)) {
    // Kundennummer formatieren
    // Die Kundennummer ist hier exemplarisch die Seriennummer des MIFARE RFID Chip genommen
    // (im HEX Format, da kürzer für Display)
    // Bei einer realen Implementation sollten die Karten vorher beschrieben werden!
    String kundennummer;
    kundennummer = String(rfid_helper.nuidPICC[0], HEX) + " " + String(rfid_helper.nuidPICC[1], HEX) + " " + String(rfid_helper.nuidPICC[2], HEX) + " " + String(rfid_helper.nuidPICC[3], HEX);

    display.mode_please_wait(kundennummer);

    double first_distance;

    if (USE_LOADCELL) {
      // Gewicht ermitteln in loadcell
      loadcell.tare();
    } else if (USE_ULTRASOUND) {
      first_distance = ultrasound.get_value();
    }

    int messure_time = millis();
    double current_weight = 0;
    float current_price = 0;

    attachInterrupt(digitalPinToInterrupt(PIN_CONTACT), ISR_contactpin_falling, FALLING);

    // Schloss öffnen
    open_lock();

    // Test begin
    while (contactpin_status == true) {
      Serial.println("closed");
    }

    Serial.println("opened");
    // Test ende

    detachInterrupt(digitalPinToInterrupt(PIN_CONTACT));

    // stellt das schließen der Box fest und ändert eine Bool Var
    attachInterrupt(digitalPinToInterrupt(PIN_CONTACT), ISR_contactpin_rising, RISING);

    // Bis die Box geschlossen wird verbleibt das Programm hier
    while (contactpin_status == false) {
      // Sicherstellen, dass nur jede Sekunde gemessen wird
      // messure_time == new_messure_time ist nur beim ersten mal gleich!
      // dies sorgt dafür, dass die erste Messung sofort erfolgt
      if (int new_messure_time = millis()-messure_time >= 1000 || messure_time == new_messure_time) {
        if (USE_LOADCELL) {
          // Gewicht kontinuierlich ermitteln
          // Zeigt dem Kunden Informationen während der Entnahme
          current_weight = abs(loadcell.get_units());
          // Preisberechnung kontinuierlich
          current_price = abs(current_weight * price_per_g);
          messure_time = new_messure_time;
        } else if (USE_ULTRASOUND) {
          double current_distance = ultrasound.get_value();
          current_weight = abs(first_distance-current_distance)*weight_per_1cm_height_g;
          current_price = abs(current_weight * price_per_g);
          messure_time = new_messure_time;
        }
        display.mode_opened_case(kundennummer, current_price, current_weight);
      }
    }

    display.mode_please_wait(kundennummer);

    detachInterrupt(digitalPinToInterrupt(PIN_CONTACT));

    double final_weight;
    float final_price;
    if (USE_LOADCELL) {
      // Gewicht neu ermitteln in loadcell
      final_weight = abs(loadcell.get_units());
      // Preisberechnung final
      final_price = abs(final_weight * price_per_g);
    } else if (USE_ULTRASOUND) {
      double final_distance = ultrasound.get_value();
      final_weight = abs(first_distance-final_distance)*weight_per_1cm_height_g;
      final_price = abs(final_weight * price_per_g);
    }

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

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1(); // bei realer Implementation andere verwenden, da bereits genackt
}