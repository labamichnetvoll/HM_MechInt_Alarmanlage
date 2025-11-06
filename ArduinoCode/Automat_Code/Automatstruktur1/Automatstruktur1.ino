/** Nützliche Links:
 * https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
 *
 *
*/



/* Bibliotheken Einbinden */
#include <Arduino.h>
#include "Ultrasonic.h"            //fertige Bibliothek für Ultraschall Sensor
#include <Arduino_LSM6DS3.h>       // IMU

/* PIN Nummer - Defintionen*/
#define ULTRASONIC_PIN_NR 7     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6       // evtl. anpassen! TODO
#define BUTTON_PIN 2            // evtl. anpassen! TODO     Auswahl Pins: 2, 3, 9, 10, 11, 13, A1, A5, A7

/* Konstanten */
#define PERIOD_UPDATE 100   // Taktlänge in ms


/* Variablen für Automaten */
long lastupdate = 0;
enum Zustaende {
  Start,
  Initialisierung,
  Auswahl_Hotel,
  Auswahl_Gepaeck,
  Auswahl_Zelt,
  Scharf_Hotel,
  Scharf_Gepaeck,
  Scharf_Zelt,
  Alarm
};

enum Zustaende Zustand;

/* Konstruktor CPP Klassen*/
Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);


void setup() {

  Serial.begin(115200);
  while (!Serial);          //TODO Später auskommentieren!
  Serial.println("Started.");
  /* IO-Init*/
  pinMode(INFRAROT_PIN_NR, INPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  //attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), ISR_NAME??, CHANGE);   //ISR Funktion hinzufügen

  /* IMU Init*/
  if (!IMU.begin()){
    Serial.println("Failed to initialize IMU!");
    while (1);    // aus ExampleCode - Simple Accelerometer
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");



  /* Automat-Init*/
  Zustand = Start;        //Zustand auf Start
  lastupdate = millis();  //Taktzähler zurücksetzen

}


void loop() {

  if (millis() - lastupdate > PERIOD_UPDATE)  {

    lastupdate = millis();  //Taktzähler zurücksetzen

    RawIMU();

    //Zustandswechsel hier
    switch(Zustand){

    case Start:
      Zustand = Initialisierung;
      break;

    case Initialisierung:
      //IR-Sensor initialisieren
      //a-Sensor initialisieren
      //Schallsensor initialisieren
      //Display initialisieren
      //ISR initialisieren
      Zustand = Auswahl_Hotel;  //Nach Abschluss: zu Auswahl_Hotel wechseln
      break;

    case Auswahl_Hotel:
      //Anzeige: Hotelmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Gepaeck wechseln
      //Falls langer Knopfdruck: Countdown, dann zu Scharf_Hotel wechseln
      break;

    case Auswahl_Gepaeck:
      //Anzeige: Gepäckmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Zelt wechseln
      //Falls langer Knopfdruck: Countdowhn, dann zu Scharf_Gepaeck wechseln
      break;

    case Auswahl_Zelt:
      //Anzeige: Zeltmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Hotel wechseln
      //Falls langer Knopfdruck: Countdown, dann zu Scharf_Zelt wechseln
      break;

    case Scharf_Hotel:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //Schallsensor abfragen
      //Falls Abstand stark verändert: zu Alarm wechseln
      break;

    case Scharf_Gepaeck:
     //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //a-Sensor abfragen
      //Falls starke Beschleunigung: zu Alarm wechseln
      break;

    case Scharf_Zelt:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //IR-Sensor abfragen
      //Falls Bewegung erkannt: zu Alarm wechseln
      break;
    
    case Alarm:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //Sonst: LED blinken, Lautsprecher aktivieren, Piezo aktivieren
      break;

    default:
      //Nur zur Fehlerbehebung, immer zu Start wechseln
      Zustand = Start;
      break;
    }
  }
}

// Funktionen hier

