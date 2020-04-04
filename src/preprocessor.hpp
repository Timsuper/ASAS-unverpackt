/*
    Hier werden alle verwendeten Arduinopins aufgeführt.

    Arduino Mega HW SPI:
    MISO - 50
    MOSI - 51
    SCK - 52
    SS/SDA - 53

    Arduino Mega SW SPI:
    siehe Display Sektion unten
*/
// Auswahl Bulk Bin oder Box Aufbau
#define USE_LOADCELL false
#define USE_ULTRASOUND true

// Box Schloss Pin
#define PIN_DOORLOCK 7
// Kontaktschalter Pin Box und Bulk Bin
#define PIN_CONTACT 18

// Ultraschall Pins ! Nur Ultraschall ODER Wägezellen Aufbau, daher können Pins doppelt verwendet werden !
#define trigPin_1 23
#define echoPin_1 24

#define trigPin_2 25
#define echoPin_2 26

// Wiezellen Pins ! Nur Ultraschall ODER Wägezellen Aufbau, daher können Pins doppelt verwendet werden !
#define LOADCELL_DOUT_PIN_1 22
#define LOADCELL_SCK_PIN_1 23

#define LOADCELL_DOUT_PIN_2 24
#define LOADCELL_SCK_PIN_2 25

// RFID
#define RFID_SS_PIN 53
#define RFID_RST_PIN 9

// Display
#define DISPLAY_CLOCK_PIN 5
#define DISPLAY_DATA_PIN 4
#define DISPLAY_SS_PIN 2
#define DISPLAY_RST_PIN 8