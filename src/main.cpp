#include <preprocessor.hpp>
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>

#include <MFRC522.h> // RFID
#include <HX711.h> // Wiegezelle
#include <U8g2lib.h> // Display

#include <rfid.hpp>
#include <display.hpp>
#include <loadcell.hpp>
#include <ultrasound.hpp>

const String product_name = "Cornflakes";

const float price_per_g = 0.0025;
const int price_per_X_g = 100;
const float weight_per_1cm_height_g = 40;

int debug_milis_timer = 0;

void open_lock(int time = 100) {
  // Schloss für time ms öffnen
  digitalWrite(PIN_DOORLOCK, true);
  delay(time);
  digitalWrite(PIN_DOORLOCK, false);
}

void setup() {
  Serial.begin(9600);
  SPI.begin();

  // Schloss Pin
  pinMode(PIN_DOORLOCK, OUTPUT);
  digitalWrite(PIN_DOORLOCK, false);

  pinMode(PIN_CONTACT, INPUT);

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

  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

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
    //kundennummer = String(rfid_helper.nuidPICC[0], HEX) + " " + String(rfid_helper.nuidPICC[1], HEX) + " " + String(rfid_helper.nuidPICC[2], HEX) + " " + String(rfid_helper.nuidPICC[3], HEX);
    kundennummer = String(rfid_helper.nuidPICC[0], DEC) + " " + String(rfid_helper.nuidPICC[1], DEC) + " " + String(rfid_helper.nuidPICC[2], DEC);

    display.mode_please_wait(kundennummer);

    double first_distance;
    double first_weight;

    if (USE_LOADCELL) {
      // Gewicht ermitteln in loadcell
      first_weight = abs(loadcell.get_units());
    } else if (USE_ULTRASOUND) {
      first_distance = ultrasound.get_one_avg_value();
      Serial.print("first_dist: ");
      Serial.print(first_distance);
    }

    int messure_time = millis();
    double current_weight = 0;
    float current_price = 0;

    // Schloss öffnen
    open_lock();

    if (USE_ULTRASOUND) {
      display.mode_take_product(kundennummer);
      while (digitalRead(PIN_CONTACT) == 0) {
          delay(20);
      }
    }

    // Bis die Box geschlossen wird verbleibt das Programm hier
    do {
      // Sicherstellen, dass nur jede Sekunde gemessen wird
      // messure_time == new_messure_time ist nur beim ersten mal gleich!
      // dies sorgt dafür, dass die erste Messung sofort erfolgt
      if (int new_messure_time = millis()-messure_time >= 1000 || messure_time == new_messure_time) {
        if (USE_LOADCELL) {
          // Gewicht kontinuierlich ermitteln
          // Zeigt dem Kunden Informationen während der Entnahme
          current_weight = abs(loadcell.get_units() - first_weight);
          // Preisberechnung kontinuierlich
          current_price = abs(current_weight * price_per_g);
          messure_time = new_messure_time;
        } else if (USE_ULTRASOUND) {
          double current_distance = ultrasound.get_one_avg_value();
          current_weight = abs(first_distance-current_distance)*weight_per_1cm_height_g;
          current_price = abs(current_weight * price_per_g);
          messure_time = new_messure_time;
        }
        display.mode_opened_case(kundennummer, current_price, current_weight);
      }
      delay(100);
    } while (digitalRead(PIN_CONTACT));

    display.mode_please_wait(kundennummer);

    double final_weight;
    double last_messure;
    double final_distance;
    float final_price;
    if (USE_LOADCELL) {
      // Gewicht neu ermitteln in loadcell
      last_messure = loadcell.get_units();
      final_weight = abs(last_messure - first_weight);
      // Preisberechnung final
      final_price = abs(final_weight * price_per_g);
    } else if (USE_ULTRASOUND) {
      final_distance = ultrasound.get_one_avg_value();
      Serial.print("  last_dist:  ");
      Serial.println(final_distance);

      final_weight = abs(first_distance-final_distance)*weight_per_1cm_height_g;
      final_price = abs(final_weight * price_per_g);
    }

    // Display Abrechnung
    display.mode_closed_case(kundennummer, final_price, final_weight);

    delay(5000);

    // Gewicht 0 oder niedrig? -> Box leer anzeigen
    if ((USE_LOADCELL && last_messure <= 100 && last_messure >= 0) || (USE_ULTRASOUND && final_distance > 40)) {
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