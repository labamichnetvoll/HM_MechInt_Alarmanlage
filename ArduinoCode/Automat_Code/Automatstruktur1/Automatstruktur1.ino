/* Nützliche Links:
 * https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
 */



/* Bibliotheken Einbinden */
#include <Arduino.h>
#include "Ultrasonic.h"            //fertige Bibliothek für Ultraschall Sensor
#include <Arduino_LSM6DS3.h>       // IMU
// #include "WebServer.h"             // Web Anteil - Später einbinden

/* PIN Nummer - Defintionen*/
#define ULTRASONIC_PIN_NR 7     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6       // evtl. anpassen! TODO
#define BUTTON_PIN 2            // evtl. anpassen! TODO     Auswahl Pins: 2, 3, 9, 10, 11, 13, A1, A5, A7

/* Konstanten */
#define PERIOD_UPDATE 100   // Taktlänge in ms


/* Variablen für Automaten */
unsigned long ulLastupdate = 0;
enum enZustaende {
  Start,
  Sensorauswahl,
  Aktiv,
  Alarm
};
enum enZustaende Zustand;

/* Sensorvariablen */
bool bIR_Sensor_an = 0;
bool bUS_Sensor_an = 0;
bool bA_Sensor_an = 0;
bool bSensor_ausgeloest = 0;

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
  ulLastupdate = millis();  //Taktzähler zurücksetzen

}


void loop() {

  if (millis() - ulLastupdate > PERIOD_UPDATE)  {   //if-Abfrage funktioniert für mindestens 7 Wochen, dann Overflow möglich

    ulLastupdate = millis();  //Taktzähler zurücksetzen

    RawIMU();

    //Zustandswechsel hier
    switch(Zustand){

    case Start:
      //Sensoren und Aktoren Initialisieren
      bIR_Sensor_an = 0;
      bUS_Sensor_an = 0;
      bA_Sensor_an = 0;
      Zustand = Sensorauswahl;
      break;
    
    case Sensorauswahl:
      //Diplay zeigt "Bereit"
      //Web UI kann Sensorvariablen ändern:
      if(bIR_Sensor_an || bUS_Sensor_an || bA_Sensor_an){
        Zustand = Aktiv;
      }
      break;

    case Aktiv:
      if(bIR_Sensor_an){
        //Sensor abfragen
      }
      if(bUS_Sensor_an){
        //Sensor abfragen
      }
      if(bA_Sensor_an){
        //Sensor abfragen
      }

      if(bSensor_ausgeloest){
        Zustand = Alarm;
      }
      break;

    case Alarm:
      //LED blinken, Lautsprecher aktivieren, Piezo aktivieren
      //Web UI kann bSensor_ausgeloest auf 0 setzen

      if(!bSensor_ausgeloest){
        Zustand = Start;
      }
      break;

    default:
      //Nur zur Fehlerbehebung, immer zu Start wechseln
      Zustand = Start;
      break;
    }
  }
  if (ulLastupdate - millis() > 0) {    //Bei Overflow: ulLastupdate neu setzen, um Takt wiederherzustellen
    ulLastupdate = millis();
  }
}

// Funktionen hier

