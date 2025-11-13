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
#define ULTRASONIC_PIN_NR 3     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 2       // evtl. anpassen! TODO
#define BUTTON_PIN 22            // evtl. anpassen! TODO     Auswahl Pins Interrupt: 2, 3, 9, 10, 11, 13, A1, A5, A7
#define LED_PIN 4               // evtl. anpassen! TODO
#define PIEZZO_PIN 5            // evtl. anpassen! TODO
#define PWM_PIN 9               // evtl. anpassen! TODO     Wichtig PWM PIN erwischen
#define SHDN_PIN 10             // evtl. anpassen! TODO     Verstärker AN/AUS
#define MUTE_PIN 11             // evtl. anpassen! TODO     Verstärker Mute  

/* Konstanten */
#define PERIOD_UPDATE 100   // Taktlänge in ms


/* globale Sensorvariablen , werden durch WebUI aktiviert*/
bool bIR_Sensor_an = 0;
bool bUS_Sensor_an = 0;
bool bA_Sensor_an = 0;
bool bSensor_ausgeloest = 0;

/* globale Sensorvariable, codeintern*/
long ldistultraschallvgl = 240;


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
  pinMode(LED_PIN, OUTPUT);
  pinMode(PIEZZO_PIN, OUTPUT);
  pinMode(PWM_PIN, OUTPUT);
  pinMode(SHDN_PIN, OUTPUT);      

  digitalWrite(SHDN_PIN, 0);    // Deaktiviert Verstärker Modul

  /* IMU Init*/
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


}

void SensorALL()  {
    // put your main code here, to run repeatedly:
  bool us_ausgeloest = 0;


  //Sensor abfragen & auswerten
  long distanceUS = ReturnUltraschall();

  if ( distanceUS - ldistultraschallvgl > 5 || distanceUS - ldistultraschallvgl < -5 )  {
    us_ausgeloest = 1;
  }
  else us_ausgeloest = 0;



  Serial.print("IR: ");
  Serial.print(ReturnInfrarot());
  Serial.print(" , US: ");
  Serial.print(us_ausgeloest);
  Serial.print(" , ACC: ");
  Serial.println(ReturnAcceleration());


}



void loop() {

  //long distance = ReturnUltraschall();
  //Serial.println(distance);

  SensorALL();

  delay(1);

}
