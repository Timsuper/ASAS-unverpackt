#include <Arduino.h>
#include <SPI.h>

#include <MFRC522.h> //RFID
#include <HX711.h> //Wiegezelle
#include <U8g2lib.h> //Display

#include <loadcell.hpp>
#include <rfid.hpp>
#include <display.hpp>

class loadcell_test_class {
private:

public:
  void loadcell_test_init() {
    loadcell.loadcell_1.begin(loadcell.LOADCELL_DOUT_PIN_1, loadcell.LOADCELL_SCK_PIN_1);
    loadcell.loadcell_2.begin(loadcell.LOADCELL_DOUT_PIN_2, loadcell.LOADCELL_SCK_PIN_2);

    if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
      loadcell.loadcell_1.set_scale();
      Serial.println("set_scale() -> init");
      loadcell.loadcell_1.tare();
      Serial.println("tare() -> zelle nullen");
      Serial.println("HX711 Nr 1 OK");
    } else {
      Serial.println("HX711 Nr 1 NOT READY");
    }

    if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
      loadcell.loadcell_2.set_scale();
      Serial.println("set_scale() -> init");
      loadcell.loadcell_2.tare();
      Serial.println("tare() -> zelle nullen");
      Serial.println("HX711 Nr 2 OK");
    } else {
      Serial.println("HX711 Nr 2 NOT READY");
    }

    Serial.println();
    Serial.println("loadcell_test_init() abgeschlossen");
    Serial.println("> HX711 commands aktiv");
  }

  void loadcell_case_handler_test() {
    String str_2;
    String str_3;

    int cmd = 0;
    String str = Serial.readString();
    if (str.indexOf("test") > -1){
      cmd = 1;
    }
    else if (/*str.indexOf("tare-n") > -1 ||*/ str.indexOf("tn") > -1) {
      cmd = 2;
    }
    else if (/*str.indexOf("tare-10") > -1 ||*/ str.indexOf("t10") > -1) {
      cmd = 3;
    }
    else if (/*str.indexOf("read-n") > -1 ||*/ str.indexOf("rn") > -1) {
      cmd = 4;
    }
    else if (/*str.indexOf("read-10") > -1 ||*/ str.indexOf("r10") > -1) {
      cmd = 5;
    }
    else if (/*str.indexOf("get-value-n") > -1 ||*/ str.indexOf("gvn") > -1) {
      cmd = 6;
    }
    else if (/*str.indexOf("get-value-10") -1 ||*/ str.indexOf("gv10") > -1) {
      cmd = 7;
    }
    else if (/*str.indexOf("get-units-n") -1 ||*/ str.indexOf("gun") > -1) {
      cmd = 8;
    }
    else if (/*str.indexOf("get-units-10") -1 ||*/ str.indexOf("gu10") > -1) {
      cmd = 9;
    }
    else if (/*str.indexOf("set-scale-a") -1 ||*/ str.indexOf("aa") > -1) {
      cmd = 10;
    }
    else if (/*str.indexOf("set-scale-b") -1 ||*/ str.indexOf("bb") > -1) {
      cmd = 11;
    }
    else if (/*str.indexOf("set-offset") -1 ||*/ str.indexOf("so") > -1) {
      cmd = 12;
    }
    Serial.print("cmd: ");
    Serial.println(cmd);

    switch (cmd)
    {
    case 1: { // cmd: test
      Serial.println("> test bestanden");
      break;
    }
    case 2: { // cmd: tare-n
      Serial.println("> command tare() (nullen mit eine Messung)");
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        loadcell.loadcell_1.tare();
        Serial.println(">> Nr 1: tare() erfolgreich");
      } else {
        Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        loadcell.loadcell_2.tare();
        Serial.println(">> Nr 2: tare() erfolgreich");
      } else {
        Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      break;
    }
    case 3: { // cmd: tare-5
      Serial.println("> command tare(10) (nullen mit 10 Messungen)");
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        loadcell.loadcell_1.tare(10);
        Serial.println(">> Nr 1: tare(10) erfolgreich");
      } else {
        Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        loadcell.loadcell_2.tare(10);
        Serial.println(">> Nr 2: tare(10) erfolgreich");
      } else {
        Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      break;
    }
    case 4: { // cmd: read-n
      Serial.println("> command read() (single A/D read)");
      int wert_1;
      int wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: read() gelesen: ");
        wert_1 = loadcell.loadcell_1.read();
        Serial.println(wert_1/2); //Ein Sensor erfasst 0,5 Fg
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: read() gelesen: ");
        wert_2 = loadcell.loadcell_2.read();
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 5: { // cmd: read-10
      Serial.println("> command read_average(10) (10x A/D read mit Mittelwert)");
      int wert_1;
      int wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: read_average(10) gelesen: ");
        wert_1 = loadcell.loadcell_1.read_average(10);
        Serial.println(wert_1/2);
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: read_average(10) gelesen: ");
        wert_2 = loadcell.loadcell_2.read_average(10);
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 6: { // cmd: get-value-n
      Serial.println("> command get_value() (1x A/D read - tare_wert)");
      float wert_1;
      float wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: get_value() gelesen: ");
        wert_1 = loadcell.loadcell_1.get_value();
        Serial.println(wert_1/2);
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: get_value() gelesen: ");
        wert_2 = loadcell.loadcell_2.get_value();
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        //Serial.println(wert_1/2+wert_2/2);
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 7: { // cmd: get-value-10
      Serial.println("> command get_value(10) (10x A/D read (Mittelwert) - tare_wert)");
      float wert_1;
      float wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: get_value(10) gelesen: ");
        wert_1 = loadcell.loadcell_1.get_value(10);
        Serial.println(wert_1/2);
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: get_value(10) gelesen: ");
        wert_2 = loadcell.loadcell_2.get_value(10);
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 8: { // cmd: get-units-n
      Serial.println("> command get_units() ((1x A/D read - tare_wert) durch scale)");
      float wert_1;
      float wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: get_units() gelesen: ");
        wert_1 = loadcell.loadcell_1.get_units();
        Serial.println(wert_1/2);
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: get_units() gelesen: ");
        wert_2 = loadcell.loadcell_2.get_units();
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 9: { // cmd: get-units-10
      Serial.println("> command get_units(10) ((10x A/D read (Mittelwert) - tare_wert) durch scale)");
      float wert_1;
      float wert_2;
      if (loadcell.loadcell_1.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 1: get_units(10) gelesen: ");
        wert_1 = loadcell.loadcell_1.get_units(10);
        Serial.println(wert_1/2);
      } else {
          Serial.println(">> HX711 Nr 1 nicht bereit");
      }
      if (loadcell.loadcell_2.wait_ready_timeout(1000)) {
        Serial.print(">> Nr 2: get_units(10) gelesen: ");
        wert_2 = loadcell.loadcell_2.get_units(10);
        Serial.println(wert_2/2);
      } else {
          Serial.println(">> HX711 Nr 2 nicht bereit");
      }
      if (wert_1 and wert_2) {
        Serial.print("Mittelwert: ");
        Serial.println((wert_1+wert_2)/2); //Mittelwert bilden
      }
      break;
    }
    case 10: { // cmd: set-scale-a
      Serial.println("> command set_scale(x)");

      Serial.println(">> bitte Faktor Nr 1 eingeben (5 sec warten)");
      delay(5000);
      String str_2_1 = Serial.readString();
      float factor_1 = atof(str_2_1.c_str());

      Serial.println(">> bitte Faktor Nr 2 eingeben (5 sec warten)");
      delay(5000);
      String str_2_2 = Serial.readString();
      float factor_2 = atof(str_2_2.c_str());

      Serial.println(">>> ist das richtig? : ");
      Serial.println("");
      Serial.print("Faktor 1: ");
      Serial.println(factor_1);
      Serial.print("Faktor 2: ");
      Serial.println(factor_2);
      Serial.println(">>>> ja oder nein ? (3 sec warten)");
      delay(3000);
      String str_3 = Serial.readString();
      if (str_3.indexOf("ja") > -1) {
        Serial.print(">>>>> habe ja erhalten");
        if (loadcell.loadcell_1.wait_ready_timeout(1000) and loadcell.loadcell_2.wait_ready_timeout(1000)) {
          Serial.println("");
          Serial.print(" -> loadcell.loadcell_1.set_scale(");
          Serial.print(factor_1);
          Serial.println(")");
          Serial.print(" -> loadcell.loadcell_2.set_scale(");
          Serial.print(factor_2);
          Serial.println(")");
          loadcell.loadcell_1.set_scale(factor_1);
          loadcell.loadcell_2.set_scale(factor_2);
        } else {
            Serial.print(" -> mind. ein HX711 nicht bereit");
        }
      } else {
        Serial.println(">>>>> habe nicht ja erhalten -> mache nichts");
      }
      break;
    } 
    case 11: { // cmd: set-scale-b
      Serial.println("> command set_scale()");
      if (loadcell.loadcell_1.wait_ready_timeout(1000) and loadcell.loadcell_2.wait_ready_timeout(1000)) {
        loadcell.loadcell_1.set_scale();
        loadcell.loadcell_2.set_scale();
        Serial.println(">> set_scale() erfolgreich");
      } else {
        Serial.println(">> mind. ein HX711 nicht bereit");
      }
      break;
    }
    case 12: { // cmd: set-offset
      Serial.println("> command set_offset(x)");
      Serial.println(">> bitte Offset eingeben");
      delay(7000);
      String str_2 = Serial.readString();
      int offset = atoi(str_2.c_str());
      Serial.print(">>> ist das richtig? : ");
      Serial.println(offset);
      Serial.println(">>>> ja oder nein ?");
      delay(4000);
      String str_3 = Serial.readString();
      if (str_3.indexOf("ja") > -1) {
        Serial.print(">>>>> habe ja erhalten");
        if (loadcell.loadcell_1.wait_ready_timeout(1000) and loadcell.loadcell_2.wait_ready_timeout(1000)) {
          Serial.println("");
          Serial.print(" -> set_offset(");
          Serial.print(offset);
          Serial.println(")");
          loadcell.loadcell_1.set_offset(offset);
          loadcell.loadcell_2.set_offset(offset);
        } else {
            Serial.print(" -> mind. HX711 nicht bereit");
        }
      } else {
        Serial.println(">>>>> habe NICHT ja erhalten, else -> mache nichts");
      }
      break;
    }
    default: {
      Serial.println("> Command nicht bekannt.");
      break;
      }
    }
    cmd = 0;
  }
} loadcell_test;

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