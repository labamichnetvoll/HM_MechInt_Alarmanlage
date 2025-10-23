#include "Ultrasonic.h"

#define ULTRASONIC_PIN_NR 7

Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);

void setup() {
  Serial.begin(9600);    // Seriellen Monitor starten mit 9600 baud
}

void loop() {
  long RangeInCentimeters;

  RangeInCentimeters = ultrasonic.MeasureInCentimeters();
  Serial.print(RangeInCentimeters);
  Serial.println(" cm");
  delay(250);
}
