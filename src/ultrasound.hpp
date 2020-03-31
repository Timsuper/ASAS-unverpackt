#include <Arduino.h>

#define trigPin 23
#define echoPin 24

class ultrasound_class {
private:
    
public:
    void init() {
        pinMode(trigPin, OUTPUT);
        pinMode(echoPin, INPUT);
    }

    double messure() {
        unsigned long duration, distance;

        cli();

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10); // laut Datenblatt muss mind ein 10 µs Signal angelegt werden
        digitalWrite(trigPin, LOW);
        duration = pulseIn(echoPin, HIGH); // Netterweise übernimmt die Arduino Bib die Zeitmessung für die Pulslänge

        sei();

        /*  Entfernung aus der Dauer berechenen
            v_schall_luft = 331,5 m/s + ( 0,6 x 20 °C ) m/s = 343,5 m/s = 0,03435 cm/µs
            Da es sich um ein Echo handelt natürlich durch 2 teilen.
        */

       distance = (duration / 2) * 0.03435;
       return distance; // in cm
    }
} ultrasound;