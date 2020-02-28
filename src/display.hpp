#include <Arduino.h>
#include <SPI.h>

#include <U8g2lib.h> //Display

U8G2_ST7920_128X64_F_SW_SPI u8g2(U8G2_R0, 13, 11, 10, 8);

class display_class {
private:

public:
  int displayheight = u8g2.getDisplayHeight();
  int displaywidth = u8g2.getDisplayWidth();

  void init() {
    u8g2.begin();
    //u8g2.setDisplayRotation(U8G2_R0);
    u8g2.setFont(u8g2_font_6x13_mf);
    u8g2.setFontPosTop();
    u8g2.setFontDirection(0);
    return;
  }

  void display_mode_normal() {
    int height_spacer = 5;
    u8g2.drawStr(5, height_spacer, "Produktname");
    u8g2.drawStr(5, height_spacer+u8g2.getAscent(), "Preis / 100g:");
    u8g2.drawStr(10, height_spacer+u8g2.getAscent()*2, "0,00 €");
    u8g2.drawLine(5, displayheight-height_spacer-2, 5, displayheight-7);
    u8g2.drawStr(5, displayheight-height_spacer, "Bitte Karte auflegen");
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