#include <Arduino.h>
#include <SPI.h>

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
    analogWrite(7, 128);

    u8g2.begin();
    return;
  }

  void prepare() {
    u8g2.clearBuffer();

    //u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(u8g2_font_6x13_mf);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    u8g2.setBitmapMode(1);
    return;
  }

  void display_mode_normal() {
    prepare();
    int height = 0;
    u8g2.drawStr(5, 0, "Produktname");

    height += u8g2.getAscent()+(-u8g2.getDescent())+3;
    u8g2.drawStr(5, height, "Preis / 100g:");

    height += u8g2.getAscent()+(-u8g2.getDescent())+3;
    u8g2.drawStr(displaywidth-u8g2.getStrWidth("0,00 Euro"), height, "0,00 Euro");

    u8g2.drawLine(5, displayheight-20, displaywidth-5, displayheight-20);
    u8g2.drawStr(5, displayheight-15, "Bitte Karte auflegen");

    u8g2.sendBuffer();
  }

  void display_mode_opened_case() {

  }

  void display_mode_closed_case() {

  }

  void display_mode_error() {

  }

  void display_mode_empty_container() {

  }
} display;