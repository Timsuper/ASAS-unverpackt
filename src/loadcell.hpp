#include <Arduino.h>
#include <SPI.h>

#include <HX711.h> //Wiegezelle

class loadcell_class {
public:
  HX711 loadcell_1;
  HX711 loadcell_2;

  const int LOADCELL_DOUT_PIN_1 = 2;
  const int LOADCELL_SCK_PIN_1 = 3;

  const int LOADCELL_DOUT_PIN_2 = 4;
  const int LOADCELL_SCK_PIN_2 = 5;
} loadcell;