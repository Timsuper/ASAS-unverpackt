#include <preprocessor.hpp>
#include <Arduino.h>
#include <SPI.h>

#include <HX711.h> //Wiegezelle

/* 
HX711 nutzt SW SPI (2 Wire)

DOUT_1 - 2
SCK_1 - 3

DOUT_2 - 4
SCK_2 - 5
*/

class loadcell_class {
private:
  // Scales eintragen
  const float cell_1_scale = 223.0017625;
  const float cell_2_scale = 208.6961375;
public:
  HX711 loadcell_1;
  HX711 loadcell_2;

  void init() {
    loadcell_1.begin(LOADCELL_DOUT_PIN_1, LOADCELL_SCK_PIN_1);
    loadcell_2.begin(LOADCELL_DOUT_PIN_2, LOADCELL_SCK_PIN_2);

    if (loadcell_1.wait_ready_timeout(1000)) {
      loadcell_1.set_scale(cell_1_scale);
      loadcell_1.tare();
      Serial.println("HX711 Nr 1 OK");
    } else {
      Serial.println("HX711 Nr 1 NOT READY");
    }

    if (loadcell_2.wait_ready_timeout(1000)) {
      loadcell_2.set_scale(cell_2_scale);
      loadcell_2.tare();
      Serial.println("HX711 Nr 2 OK");
    } else {
      Serial.println("HX711 Nr 2 NOT READY");
    }

    // Scales eintragen

    Serial.println();
    Serial.println("loadcell_init() abgeschlossen");
  }

  void tare(int samples = 10) {
    // Nullt die Zellen, indem der Offset mit x samples ermittelt wird
    if (loadcell_1.wait_ready_timeout(1000)) {
      loadcell_1.tare(samples);
    } else {
      Serial.println(">> HX711 Nr 1 ERROR");
    }
    if (loadcell_2.wait_ready_timeout(1000)) {
      loadcell_2.tare(samples);
    } else {
      Serial.println(">> HX711 Nr 2 ERROR");
    }
  }

  double read(int samples = 10) {
    // Liest den A/D Wert der Zellen direkt aus (pur)
    int wert_1;
    int wert_2;
    if (loadcell_1.wait_ready_timeout(1000)) {
      wert_1 = loadcell_1.read_average(samples);
    } else {
        Serial.println(">> HX711 Nr 1 ERROR");
    }
    if (loadcell_2.wait_ready_timeout(1000)) {
      wert_2 = loadcell_2.read_average(samples);
    } else {
        Serial.println(">> HX711 Nr 2 ERROR");
    }
    if (wert_1 and wert_2) {
      return (wert_1 + wert_2) / 2;
    } else return -1;
  }

  double get_value(int samples = 10) {
    // A/D Wert minus tare (also A/D Wert)
    float wert_1;
    float wert_2;
    if (loadcell_1.wait_ready_timeout(1000)) {
      wert_1 = loadcell_1.get_value(samples);
    } else {
      Serial.println(">> HX711 Nr 1 ERROR");
    }
    if (loadcell_2.wait_ready_timeout(1000)) {
      wert_2 = loadcell_2.get_value(samples);
    } else {
      Serial.println(">> HX711 Nr 2 ERROR");
    }
    if (wert_1 and wert_2) {
      return (wert_1 + wert_2) / 2;
    } else return -1;
  }

  double get_units(int samples = 10) {
    // A/D Wert minus tare mal Faktor (Faktoren müssen in Gramm umrechnen)
    float wert_1;
    float wert_2;
    if (loadcell_1.wait_ready_timeout(1000)) {
      wert_1 = loadcell_1.get_units(samples);
    } else {
      Serial.println(">> HX711 Nr 1 ERROR");
    }
    if (loadcell_2.wait_ready_timeout(1000)) {
      wert_2 = loadcell_2.get_units(samples);
    } else {
      Serial.println(">> HX711 Nr 2 ERROR");
    }
    if (wert_1 and wert_2) {
      return (wert_1 + wert_2) / 2;
    } else return -1;
  }

  bool set_scale(int scale_factor_1 = 1, int scale_factor_2 = 1) {
    // Bei Speicher-Problemen kann diese Funktion entfernt werden
    if (loadcell_1.wait_ready_timeout(1000) and loadcell_2.wait_ready_timeout(1000)) {
      loadcell_1.set_scale(scale_factor_1);
      loadcell_2.set_scale(scale_factor_2);
      return true;
    } else {
      Serial.print(" -> mind. ein HX711 ERROR");
      return false;
    }
  }
} loadcell;