/* Bibliotheken Einbinden */
#include <Arduino.h>
#include "Ultrasonic.h"            //fertige Bibliothek für Ultraschall Sensor
#include <Arduino_LSM6DS3.h>       // IMU
// #include "WebServer.h"             // Web Anteil - Später einbinden

/* PIN Nummer - Defintionen*/
#define ULTRASONIC_PIN_NR 7     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6       // evtl. anpassen! TODO
#define BUTTON_PIN 2            // evtl. anpassen! TODO     Auswahl Pins Interrupt: 2, 3, 9, 10, 11, 13, A1, A5, A7
#define LED_PIN 4               // evtl. anpassen! TODO
#define PIEZZO_PIN 12            // evtl. anpassen! TODO
#define PWM_PIN 9               // evtl. anpassen! TODO     Wichtig PWM PIN erwischen
#define SHDN_PIN 10             // evtl. anpassen! TODO     Verstärker AN/AUS
#define MUTE_PIN 11             // evtl. anpassen! TODO     Verstärker Mute  

/* Konstanten */
#define PERIOD_UPDATE 100   // Taktlänge in ms

void AlarmOutput();

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial);          //TODO Später auskommentieren!
  Serial.println("Started.");
  /* IO-Init*/

  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_NAME??, CHANGE);   //ISR Funktion hinzufügen
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIEZZO_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(SHDN_PIN, OUTPUT);      

  digitalWrite(SHDN_PIN, 0);    // Deaktiviert Verstärker Modul

  // Lautsprecher Konfig


}

void loop() {
  // put your main code here, to run repeatedly:
  AlarmOutput();
  delay(100);

}


//LED blinken, Lautsprecher aktivieren, Piezo aktivieren
void AlarmOutput(){


  
  static unsigned long takt = 0;

  Serial.println(takt);

  digitalWrite(SHDN_PIN, 1);      // Verstärker AN

  if (takt % 2) {
    Serial.println("Piezzo");
    digitalWrite(PIEZZO_PIN, 1);   // PIEZZEO AN
  }
  else {
    digitalWrite(PIEZZO_PIN, 0);   // PIEZZEO AUS
  }
  
  analogWrite(PWM_PIN, 50);      // 0-255: mitte am lautesten
 

  if (takt < 20){
    digitalWrite(LED_PIN, 1);   // LED AN
    
  }
  else if (takt < 40){
    digitalWrite(LED_PIN, 0);   // LED AUS    

  }
  else {
    takt = 0;
  }
  
  takt++;


}