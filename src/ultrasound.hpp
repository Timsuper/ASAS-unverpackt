#include <Arduino.h>

class ultrasound_class {
private:
    
public:
    const int allowed_sensor_tolerance = 5; // erlaubter maximaler Unterschied, bevor eine Messreihe ungültig ist

    void init() {
        pinMode(trigPin_1, OUTPUT);
        pinMode(echoPin_1, INPUT);

        pinMode(trigPin_2, OUTPUT);
        pinMode(echoPin_2, INPUT);

        Serial.println("Ultrasound init() abgeschlossen");
    }

    double messure(int trigPin, int echoPin) {
        unsigned long duration, distance;

        cli();

        digitalWrite(trigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin, HIGH);
        delayMicroseconds(10); // laut Datenblatt muss mind ein 10 µs Signal angelegt werden
        digitalWrite(trigPin, LOW);
        /*
            Die Ultraschallsensoren vom Typ HC-SR04 senden nach 250 µs fuer ca. 200 µs einen Ultraschall-Burst.
            pulseIn(pin, value, timeout) detektiert nur den Start (LOW zu HIGH) und den Rückgang auf LOW.
            Daher ist es nicht notwendig ein 250+200 µs delayMicroseconds einzubauen, pulseIn() erkennt die Flanken selbst.
            Sollte der timeout ueberschritten werden liefert pulseIn 0. Beim HCSR04 liegt kein Echo vor,
            wenn nach mehr als 200 ms kein Puls (auf LOW) festgestellt werden konnte (Fehlerfall).
            Andere Fehler liegen vor, wenn beide Sensoren zu unterschiedliche Werte messen, oder gaenzlich unmoegliche Werte liefern.
        */
        duration = pulseIn(echoPin, HIGH, 200000); // Ermittlung der Pulslaenge

        sei();

        /*  Entfernung aus der Dauer berechenen
            v_schall_luft = 331,5 m/s + ( 0,6 x 20 °C ) m/s = 343,5 m/s = 0,03435 cm/µs
            Da es sich um ein Echo handelt natuerlich durch 2 teilen.
        */

       distance = (duration / 2) * 0.03435;
       return distance; // in cm
    }

    double get_value(unsigned int samples = 5) {
        int messurements_sensor_1[samples];
        int messurements_sensor_2[samples];

        bool invalid_data_pairs[samples];
        unsigned int counter_invalid = 0;

        double sum_1 = 0;
        double sum_2 = 0;

        for (int i = 0; i < samples; i++) {
            messurements_sensor_1[i] = messure(trigPin_1, echoPin_1);
            delay(10);
            messurements_sensor_2[i] = messure(trigPin_2, echoPin_2);
            delay(10);
            
            if (abs(messurements_sensor_1[i] - messurements_sensor_2[i]) >= allowed_sensor_tolerance) {
                invalid_data_pairs[i] = true;
                counter_invalid++;
            } else {
                invalid_data_pairs[i] = false;
            }

            if (invalid_data_pairs[i] == false) {
                sum_1 += messurements_sensor_1[i];
                sum_2 += messurements_sensor_2[i];
            }
        }
        
        if (counter_invalid > samples/4 && false) {
            Serial.println();
            Serial.println("too many invalid ultrasound messures");
            Serial.print("counter_invalid_messures: ");
            Serial.println(counter_invalid);
            return -1;
        } else {
            return (sum_1 + sum_2) / (samples * 2);
        }
    }
} ultrasound;