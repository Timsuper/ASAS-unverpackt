#include <Arduino.h>
#include <SPI.h>
#include <string.h>

#include <U8g2lib.h> //Display

/*
V0,Contrast - D7 (PWM)
ClockSelect, RS, D/I - D53
Reset - D8
*/

//Full Buffer
U8G2_ST7920_128X64_F_HW_SPI u8g2(U8G2_R2, 53, 8);

//Picture Loop
// U8G2_ST7920_128X64_1_HW_SPI u8g2(U8G2_R0, /* CS=*/ 53, /* reset=*/ 8);

class display_class {
private:

public:
  int displayheight = u8g2.getDisplayHeight();
  int displaywidth = u8g2.getDisplayWidth();

  void init() {
    Serial.print("height: ");
    Serial.print(displayheight);
    Serial.print(" width: ");
    Serial.println(displaywidth);

    pinMode(7, OUTPUT);
    analogWrite(7, 185);

    u8g2.begin();
    return;
  }

  void prepare() {
    u8g2.clearBuffer();

    //u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(u8g2_font_6x13_me);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.setBitmapMode(1);
    return;
  }

  void display_mode_normal(String productname = "Produktname", int price_per_X_g = 100, float price = 0.00, String lower_text = "Bitte Karte auflegen") {
    prepare();

    String price_per_X_str;
    if (price_per_X_g == 1000) {
      price_per_X_str = "1kg";
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
    u8g2.setFont(u8g2_font_6x13_me);
    height += text_height+3;

    u8g2.drawStr(5, height, ("Preis / " + price_per_X_str + ":").c_str());
    height += text_height+3;

    price_str = price_str + " Euro";
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(price_str.c_str()), height, price_str.c_str());
    
    u8g2.drawLine(5, displayheight-text_height-4, displaywidth-5, displayheight-text_height-4);
    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth(lower_text.c_str())/2, displayheight-text_height-2, lower_text.c_str());

    u8g2.sendBuffer();
  }

  void display_mode_opened_case(String kundennummer, float estimated_price) {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    String estimated_price_str;
    estimated_price_str = String(estimated_price, 2);
    estimated_price_str.replace(".", ",");

    int height = 0;

    u8g2.drawStr(5, 0, "Kundennr:");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth(kundennummer.c_str()), height, kundennummer.c_str());
    height += text_height+1;

    u8g2.drawStr(5, height, "Preis: ");
    u8g2.drawStr(displaywidth-u8g2.getStrWidth((estimated_price_str + " Euro").c_str()), height, (estimated_price_str + " Euro").c_str());
    height += text_height+10;

    u8g2.drawFrame(2, height, displaywidth-4, (text_height+1)*2+4);
    height += 2;

    u8g2.drawStr(5, height, "Produkt entnehmen");
    height += text_height+1;

    u8g2.drawStr(5, height, "und Box schliessen.");

    u8g2.sendBuffer();
  }

  void display_mode_closed_case(String kundennummer = "none", float price = 0.00, int weight_in_g = 0) {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    String price_str;
    price_str = String(price, 2);
    price_str.replace(".", ",");

    String weight_str;
    weight_str = String(weight_in_g, 10);

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

  void display_mode_error() {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawFrame(2, height, displaywidth-2, 4+4*(text_height+1)+12);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Fehler an der Anlage")/2, height, "Fehler an der Anlage");
    height += text_height+2;

    u8g2.drawLine(2, height, displaywidth-2, height);
    height += 10;
    
    u8g2.drawStr(5, height, "Wir versuchen das");
    height += text_height+1;

    u8g2.drawStr(5, height, "Problem umgehend zu");
    height += text_height+1;

    u8g2.drawStr(5, height, "loesen.");

    u8g2.sendBuffer();
  }

  void display_mode_empty_container() {
    prepare();
    const int text_height = u8g2.getAscent()+(-u8g2.getDescent());

    int height = 0;

    u8g2.drawFrame(2, height, displaywidth-2, 4+4*(text_height+1)+12);
    height += 2;

    u8g2.drawStr(displaywidth/2-u8g2.getStrWidth("Box leer")/2, height, "Box leer");
    height += text_height+2;

    u8g2.drawLine(2, height, displaywidth-2, height);
    height += 10;
    
    u8g2.drawStr(5, height, "Ein Mitarbeiter");
    height += text_height+1;

    u8g2.drawStr(5, height, "wird fuer Nachschub");
    height += text_height+1;

    u8g2.drawStr(5, height, "sorgen.");

    u8g2.sendBuffer();
  }
} display;