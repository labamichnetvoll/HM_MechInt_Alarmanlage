/* Nützliche Links:
 * https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
 */



/* Bibliotheken Einbinden */
#include <Arduino.h>
#include "Ultrasonic.h"            //fertige Bibliothek für Ultraschall Sensor
#include <Arduino_LSM6DS3.h>       // IMU
// #include "WebServer.h"             // Web Anteil - Später einbinden

/* PIN Nummer - Defintionen */
//Auswahl Pins Interrupt: 2, 3, 9, 10, 11, 13, A1, A5, A7
#define ULTRASONIC_PIN_NR 7     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6       // evtl. anpassen! TODO
#define LED_PIN 4               // Pin D4
#define PIEZZO_PIN 5            // Pin D5 !! Vin benutzen, nicht 5V !!

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

/* globale Sensorvariablen , werden durch WebUI aktiviert*/
bool bIR_Sensor_an = 0;
bool bUS_Sensor_an = 0;
bool bA_Sensor_an = 0;
bool bSensor_ausgeloest = 0;

/* globale Sensorvariable, codeintern*/
long ldistultraschallvgl = 0;

/* Konstruktor CPP Klassen*/
Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);

/* Funktionsprototypen*/
void AlarmOutput();

void setup() {

  Serial.begin(115200);
  //while (!Serial);          //TODO Später auskommentieren!
  Serial.println("Started.");

  /* IO-Init */
  pinMode(INFRAROT_PIN_NR, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIEZZO_PIN, OUTPUT);

  /* IMU Init */
  if (!IMU.begin()){
    Serial.println("Failed to initialize IMU!");
    while (1);    // aus ExampleCode - Simple Accelerometer
  }
  Serial.print("Accelerometer sample rate = ");
  Serial.print(IMU.accelerationSampleRate());
  Serial.println(" Hz");

  //Test ob Ultraschall Sensor angeschlossen und rdy
  if (ReturnUltraschall() == 0){
    Serial.println("Ultraschall defekt");
  }

  /* Automat-Init */
  Zustand = Start;        //Zustand auf Start
  ulLastupdate = millis();  //Taktzähler zurücksetzen

}


void loop() {
  
  if (millis() - ulLastupdate > PERIOD_UPDATE)  {   //if-Abfrage funktioniert für mindestens 7 Wochen, dann Overflow möglich

    ulLastupdate = millis();  //Taktzähler zurücksetzen

    ReturnAcceleration();   //TEST

    //Serial.print("Aktueller Zustand: ");
    //Serial.println(Zustand);

    //Zustandswechsel hier
    switch(Zustand){
      

      case Start:
        // Alarm ausschalten
        
        //Sensoren und Aktoren Initialisieren
        bIR_Sensor_an = 0;
        bUS_Sensor_an = 0;
        bA_Sensor_an = 0;
        Zustand = Sensorauswahl;
        break;
      
      case Sensorauswahl:
        //Diplay zeigt "Bereit"
        // (Sensorvariablen lesen aus WebUI änderungen):
        if(bIR_Sensor_an || bUS_Sensor_an || bA_Sensor_an){
          ldistultraschallvgl = ReturnUltraschall();
          Zustand = Aktiv;
        }

        
        break;

      case Aktiv:
      //Infrarotsensor
        if(bIR_Sensor_an){
          //Sensor abfragen
          bSensor_ausgeloest = ReturnInfrarot();
        }
        //Ultraschallsensor
        if(bUS_Sensor_an){
          //Sensor abfragen & auswerten
          if ( ReturnUltraschall() - ldistultraschallvgl > 5 || ReturnUltraschall() - ldistultraschallvgl < -5 )  {
            bSensor_ausgeloest = 1;
          }
        }
        //Beschleunigungssensor
        if(bA_Sensor_an){
          //Sensor abfragen
          bSensor_ausgeloest = ReturnAcceleration();
        }
        //Falls ein Sensor auslöst
        if(bSensor_ausgeloest){
          Zustand = Alarm;
        }
        break;

      case Alarm:
        //LED blinken, Piezo aktivieren
        //Web UI kann bSensor_ausgeloest auf 0 setzen
        AlarmOutput();

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
    
    
  if (ulLastupdate > millis() ) {    //Bei Overflow: ulLastupdate neu setzen, um Takt wiederherzustellen
    ulLastupdate = millis();
  }
  
}

// Funktionen hier

/* LED blinken, Piezo aktivieren */
void AlarmOutput(){

  static unsigned long takt = 0;
  Serial.println(takt);
  
  if (takt < 200){
    digitalWrite(LED_PIN, 1);       // LED AN
    digitalWrite(PIEZZO_PIN, 1);    // PIEZZO AN
    Serial.println(AN);             // Debug
  }
  else if (takt < 400){
    digitalWrite(LED_PIN, 0);       // LED AUS    
    digitalWrite(PIEZZO_PIN, 0);    // PIEZZO AUS
    Serial.println(AUS);            // Debug
  }
  else takt = 0;
  takt++;
}
