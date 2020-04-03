#include <preprocessor.hpp>
#include <Arduino.h>
#include <SPI.h>
#include <string.h>

#include <U8g2lib.h> //Display

/*
SPI:
MISO - nicht belegt
MOSI/RW - 51
SCK - 52

frei waelbar:
V0, Contrast - D7 (PWM / Poti)
CS, RS, D/I, SS, SDA - D53
RST - D8
*/

//Full Buffer
#if USE_ULTRASOUND == true
  U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R2, DISPLAY_SS_PIN, DISPLAY_RST_PIN);
#else
  U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R0, DISPLAY_SS_PIN, DISPLAY_RST_PIN);
#endif

//Picture Loop
// U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 53, /* reset=*/ 8);

class display_class {
private:

public:
  int displayheight = u8g2.getDisplayHeight();
  int displaywidth = u8g2.getDisplayWidth();

  void init() {
    Serial.print("display -> ");
    Serial.print("height: ");
    Serial.print(displayheight);
    Serial.print(" width: ");
    Serial.println(displaywidth);

    u8g2.begin();
    Serial.println("display init() abgeschlossen");
    return;
  }

  void prepare() {
    u8g2.clearBuffer();

    //u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(u8g2_font_6x13_me);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.setBitmapMode(1);
    u8g2.setFontMode(1);

    return;
  }

  void mode_normal(String productname = "Produktname", int price_per_X_g = 100, float price = 0.00, String lower_text = "Bitte Karte auflegen") {
    prepare();
    String price_per_X_str;
    if (price_per_X_g == 1000) {
      price_per_X_str = "kg";
    } else {
      price_per_X_str = String(price_per_X_g, 10);
      price_per_X_str = price_per_X_str + "g";
    }

    String price_str;
    price_str = String(price, 2);
    price_str.replace(".", ",");

    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());
    int height = 0;
    
    u8g2.setFont(u8g2_font_7x14B_mf);
    u8g2.drawStr(5, 0, productname.c_str());
    height += u8g2.getAscent()+(-u8g2.getDescent())+4;

    u8g2.setFont(u8g2_font_6x13_me);

    u8g2.drawStr(5, height, ("Preis / " + price_per_X_str + ":").c_str());
    height += text_height+5;

    price_str = price_str + " Euro";
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(price_str.c_str()), height, price_str.c_str());
    
    u8g2.drawLine(5, displayheight-text_height-4, displaywidth-5, displayheight-text_height-4);
    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth(lower_text.c_str())/2, displayheight-text_height-2, lower_text.c_str());

    u8g2.sendBuffer();
  }

  void mode_opened_case(String kundennummer = "KdNr", float estimated_price = 0.00, int estimated_weight = 0) {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    String estimated_price_str;
    estimated_price_str = String(estimated_price, 2);
    estimated_price_str.replace(".", ",");

    int height = 0;

    u8g2.drawStr(5, 0, "Kundennr:");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(kundennummer.c_str()), height, kundennummer.c_str());
    height += text_height+1;

    if (USE_ULTRASOUND) {
      u8g2.drawStr(5, height, "Preis: ");
      u8g2.drawStr(displaywidth-u8g2.getStrWidth((estimated_price_str + " Euro").c_str()), height, (estimated_price_str + " Euro").c_str());
      height += text_height+1;

      u8g2.drawStr(5, height, "Gewicht: ");
      u8g2.drawStr(displaywidth-u8g2.getStrWidth((String(estimated_weight) + " g").c_str()), height, (String(estimated_weight) + " g").c_str());
      height += text_height+1;
    }

    u8g2.drawFrame(0, height, displaywidth, displayheight-height);
    height += 1;

    u8g2.drawStr(5, height, "Produkt entnehmen");
    height += text_height+1;

    if (USE_ULTRASOUND) {
      u8g2.drawStr(5, height, "dann Hebel loslassen.");
      height += text_height+1;
    }

    if (USE_LOADCELL) {
      u8g2.drawStr(5, height, "und Box schliessen.");
      height += text_height+1;
    }

    u8g2.sendBuffer();
  }

  void mode_please_wait(String kundennummer = "KdNr") {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawStr(5, 0, "Kundennr:");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(kundennummer.c_str()), height, kundennummer.c_str());
    height += text_height+1;

    height ++;
    u8g2.drawFrame(10, height, displaywidth-20, height+text_height);
    height += 5;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Einen Moment")/2, height, "Einen Moment");
    height += text_height+1;

    u8g2.sendBuffer();
  }

  void mode_take_product(String kundennummer = "KdNr") {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawStr(5, 0, "Kundennr:");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(kundennummer.c_str()), height, kundennummer.c_str());
    height += text_height+1;

    height ++;
    u8g2.drawFrame(10, height, displaywidth-20, height+text_height);
    height += 5;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Hebel ziehen")/2, height, "Hebel ziehen");
    height += text_height+1;

    u8g2.sendBuffer();
  }

  void mode_closed_case(String kundennummer = "KdNr", float price = 0.00, int weight_in_g = 0) {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    String price_str;
    price_str = String(abs(price), 2);
    price_str.replace(".", ",");

    String weight_str;
    weight_str = String(abs(weight_in_g), 10);

    int height = 0;

    u8g2.drawStr(5, 0, "Kundennr:");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(kundennummer.c_str()), height, kundennummer.c_str());
    height += text_height+1;

    u8g2.drawStr(5, height, "Preis: ");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth((price_str + " Euro").c_str()), height, (price_str + " Euro").c_str());
    height += text_height+1;

    u8g2.drawStr(5, height, "Gewicht: ");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth((weight_str + " g").c_str()), height, (weight_str + " g").c_str());
    height += text_height+10;

    u8g2.drawFrame(15, height, displaywidth-30, (text_height+1)*1+4);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Vielen Dank!")/2, height, "Vielen Dank!");

    u8g2.sendBuffer();
  }

  void mode_error(String error_code = "") {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawFrame(0, 0, displaywidth, displayheight);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Fehler an der Anlage")/2, height, "Fehler an der Anlage");
    height += text_height+2;

    u8g2.drawLine(0, height, displaywidth, height);
    height += 2;
    
    u8g2.drawStr(5, height, "Wir versuchen das");
    height += text_height+1;

    u8g2.drawStr(5, height, "Problem umgehend zu");
    height += text_height+1;

    u8g2.drawStr(5, height, "beheben.");
    height += text_height+1;

    u8g2.drawStr(5, height, error_code.c_str());

    u8g2.sendBuffer();
  }

  void mode_unknown_card() {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawFrame(0, 0, displaywidth, displayheight);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Karte unbekannt")/2, height, "Karte unbekannt");
    height += text_height+2;

    u8g2.drawLine(0, height, displaywidth, height);
    height += 2;
    
    u8g2.drawStr(5, height, "Diese Karte ist");
    height += text_height+1;

    u8g2.drawStr(5, height, "nicht im System");
    height += text_height+1;

    u8g2.drawStr(5, height, "hinterlegt.");

    u8g2.sendBuffer();
  }

  void mode_empty_container() {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawFrame(0, 0, displaywidth, displayheight);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Box leer")/2, height, "Box leer");
    height += text_height+2;

    u8g2.drawLine(0, height, displaywidth, height);
    height += 2;
    
    u8g2.drawStr(5, height, "Ein Mitarbeiter");
    height += text_height+1;

    u8g2.drawStr(5, height, "wird Ware");
    height += text_height+1;

    u8g2.drawStr(5, height, "nachlegen.");

    u8g2.sendBuffer();
  }
} display;