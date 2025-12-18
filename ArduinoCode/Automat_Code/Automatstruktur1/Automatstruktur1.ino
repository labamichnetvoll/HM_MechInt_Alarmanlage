/* Nützliche Links:
 * https://docs.arduino.cc/language-reference/en/functions/external-interrupts/attachInterrupt/
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
 */

/* Bibliotheken Einbinden */
#include <Arduino.h>
#include "Ultrasonic.h"            // fertige Bibliothek für Ultraschall Sensor
#include <Arduino_LSM6DS3.h>       // IMU
#include <SPI.h>                   // für WiFi
#include <WiFiNINA.h>              // WLAN / Webserver
#include <Adafruit_GFX.h>          // Display
#include <Adafruit_SSD1306.h>      // Display
#include <Wire.h>                  // I2C Display
//#include "WebServer.h"           // Web Anteil - aktuell nicht benutzt

/* PIN Nummer - Defintionen */
//Auswahl Pins Interrupt: 2, 3, 9, 10, 11, 13, A1, A5, A7
#define ULTRASONIC_PIN_NR 7        // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6          // evtl. anpassen! TODO
#define LED_PIN 4                  // Pin D4
#define PIEZZO_PIN 5               // Pin D5 !! Vin benutzen, nicht 5V !!

/* Konstanten */
#define PERIOD_UPDATE 100          // Taktlänge in ms
#define SENSI 0.01                 // Sensitivity Beschleunigungssensor

#define OLED_RESET 4               // Display Reset
#define OLED_WIDTH 128             // Display Breite
#define OLED_HEIGHT 64             // Display Höhe

/* Display Init und QR-Code*/
Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);
const unsigned char qrCode[] PROGMEM = {
  0b00000000, 0b00000011, 0b11110011, 0b00111100, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b11111100, 0b11001111, 0b00001100, 0b00000000, 0b00000011, 0b11111111,
0b00110011, 0b00111100, 0b11000011, 0b00111111, 0b11110000, 0b11111111, 0b11001100, 0b11001111, 0b00110000, 0b11001111, 0b11111100, 0b00110000, 0b00110011, 0b11111111, 
0b00001111, 0b11110011, 0b00000011, 0b00001100, 0b00001100, 0b11111111, 0b11000011, 0b11111100, 0b11000000, 0b11000011, 0b00000011, 0b00110000, 0b00110000, 0b00001111, 
0b00110000, 0b00110000, 0b11000000, 0b11001100, 0b00001100, 0b00000011, 0b11001100, 0b00001100, 0b00110000, 0b00110011, 0b11001111, 0b11111111, 0b00110011, 0b00000011, 
0b00001100, 0b00001100, 0b11110011, 0b11111111, 0b11001100, 0b11000000, 0b11000011, 0b11111111, 0b00110000, 0b11000000, 0b11111111, 0b00111111, 0b11110000, 0b11111111, 
0b11001100, 0b00110000, 0b00111111, 0b11001111, 0b11111100, 0b00000000, 0b00000011, 0b00110011, 0b00110011, 0b00110000, 0b00000000, 0b00000000, 0b00000000, 0b11001100, 
0b11001100, 0b11001100, 0b00000000, 0b00001111, 0b11111111, 0b11111100, 0b00001111, 0b00111111, 0b11111111, 0b11111111, 0b11111111, 0b11111111, 0b00000011, 0b11001111, 
0b11111111, 0b11111111, 0b00000000, 0b00110000, 0b00001100, 0b11111100, 0b11001100, 0b11001100, 0b11000000, 0b00001100, 0b00000011, 0b00111111, 0b00110011, 0b00110011, 
0b00111111, 0b11001100, 0b11001111, 0b00110011, 0b00001100, 0b00111111, 0b11110011, 0b11110011, 0b00110011, 0b11001100, 0b11000011, 0b00001111, 0b11111100, 0b11001111, 
0b11000011, 0b00110011, 0b11001111, 0b00110000, 0b00110011, 0b11110011, 0b11110000, 0b11001100, 0b11110011, 0b11001100, 0b00001100, 0b11111111, 0b11000011, 0b11111111, 
0b11110000, 0b11111100, 0b11111111, 0b11111111, 0b11110000, 0b11111111, 0b11111100, 0b00111111, 0b00111111, 0b11111111, 0b11001111, 0b11000000, 0b00000011, 0b00000000, 
0b11111100, 0b00001100, 0b00110011, 0b11110000, 0b00000000, 0b11000000, 0b00111111, 0b00000011, 0b00000000, 0b11001111, 0b11000000, 0b11111111, 0b00110000, 0b00000011, 
0b11001100, 0b00110011, 0b11110000, 0b00111111, 0b11001100, 0b00000000, 0b11110011, 0b00110011, 0b00110011, 0b00001100, 0b00111100, 0b00110011, 0b00110011, 0b00001100, 
0b11001100, 0b11000011, 0b00001111, 0b00001100, 0b11001100, 0b11000011, 0b11000011, 0b11110000, 0b11001111, 0b11111100, 0b11111111, 0b11000000, 0b11110000, 0b11111100, 
0b00110011, 0b11111111, 0b00111111, 0b11110000, 0b00110000, 0b11000000, 0b00001100, 0b11111100, 0b00000000, 0b00001111, 0b00001100, 0b00110000, 0b00000011, 0b00111111, 
0b00000000, 0b00000011, 0b11001111, 0b11111111, 0b11110011, 0b00110000, 0b11110011, 0b11110011, 0b11110011, 0b11111111, 0b11111100, 0b11001100, 0b00111100, 0b11111100, 
0b11111100, 0b00000000, 0b00000011, 0b00110011, 0b11000000, 0b00110011, 0b00110000, 0b00000000, 0b00000000, 0b11001100, 0b11110000, 0b00001100, 0b11001100, 0b00000011, 
0b11111111, 0b00111111, 0b00110000, 0b11000011, 0b11110000, 0b11111100, 0b11111111, 0b11001111, 0b11001100, 0b00110000, 0b11111100, 0b00111111, 0b00110000, 0b00110011, 
0b00000011, 0b00110000, 0b00000000, 0b00001111, 0b11001100, 0b00001100, 0b11000000, 0b11001100, 0b00000000, 0b00000011, 0b11110011, 0b00000011, 0b00110000, 0b11111111, 
0b11110011, 0b00000000, 0b11000000, 0b11000000, 0b11001100, 0b00111111, 0b11111100, 0b11000000, 0b00110000, 0b00110000, 0b00110011, 0b00000000, 0b00111100, 0b11111100, 
0b00110011, 0b00001100, 0b00001100, 0b11000000, 0b00001111, 0b00111111, 0b00001100, 0b11000011, 0b11111111, 0b00110011, 0b11001111, 0b00000011, 0b00110000, 0b00001100, 
0b11111111, 0b11001100, 0b11110011, 0b11000000, 0b11001100, 0b00000011, 0b00000000, 0b00000011, 0b00111100, 0b11111111, 0b00111111, 0b11000000, 0b00000000, 0b00000000, 
0b11001111, 0b00111111, 0b11001111, 0b11110000, 0b0000
};
    //Array mit QR-Code Grafik NICHT FUNKTIONAL TODO

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

/* globale Sensorvariablen, codeintern */
long ldistultraschallvgl = 0;             //Vergleichs-Abstand für Ultraschallmessung
unsigned long dauer = 0;                  //zählt Anzahl der Piepser im Alarm / wird für das Blinken des QR-Codes missbraucht
float acc_x = 0;                          //aktuelle Beschleunigungswerte
float acc_y = 0;
float acc_z = 0;
float acc_x_old = 0;                      //alte Beschleunigungswerte
float acc_y_old = 0;
float acc_z_old = 0;

// Webserver / WLAN
const char ssid[] = "ReiseAlarm_AP";     // Name des WLANs
const char pass[] = "12345678";          // Passwort (mind. 8 Zeichen)

WiFiServer server(80);

// Zustand nur für Web-Anzeige (Start/Stop)
bool ueberwachungAktiv = false;

/* Konstruktor CPP Klassen */
Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);

/* Funktionsprototypen */
void AlarmOutput();
void WebServer_begin();
void WebServer_handleClient();
void sendStatus(WiFiClient client);
void sendWebApp(WiFiClient client);
void displayQRcodeAnzeigen();

// Sensor-Funktionen – hier als Prototypen, unten implementiert:
long ReturnUltraschall();
bool ReturnInfrarot();
bool ReturnAcceleration();

/* =======================================================================
 *  SETUP
 * ======================================================================= */
void setup() {
  /* Display Init */
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);

  // Display zeigt "startet.."
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(18, 5);
  display.println("Reisealarmanlage");
  display.setTextSize(2);
  display.setCursor(12, 31);
  display.println("startet..");
  display.display();
  display.invertDisplay(false);


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
    Serial.println("Ultraschall defekt (Distanz = 0)");
  }

  /* Automat-Init */
  Zustand = Start;        //Zustand auf Start
  ulLastupdate = millis();  //Taktzähler zurücksetzen

  // Webserver starten (Access Point + HTTP-Server)
  WebServer_begin();
}

/* =======================================================================
 *  LOOP
 * ======================================================================= */
void loop() {
  
  // HTTP-Anfragen vom Web-Client bearbeiten
  WebServer_handleClient();
  
  if (millis() - ulLastupdate > PERIOD_UPDATE)  {   //if-Abfrage funktioniert für mindestens 7 Wochen, dann Overflow möglich

    ulLastupdate = millis();  //Taktzähler zurücksetzen

    ReturnAcceleration();   // TEST

    //Zustandswechsel hier
    switch(Zustand){

      case Start:
        // Alarm ausschalten
        digitalWrite(LED_PIN, 0);       // LED AUS    
        digitalWrite(PIEZZO_PIN, 0);    //Piezo AUS
        dauer = 0;                      //Piepser-zähler reset
        
        //Sensoren und Aktoren Initialisieren
        bIR_Sensor_an = 0;
        bUS_Sensor_an = 0;
        bA_Sensor_an = 0;
        bSensor_ausgeloest = 0;

        Zustand = Sensorauswahl;
        break;
      
      case Sensorauswahl:
        //QR-Code und "Bereit" im Wechsel
        if(dauer <= 15){
          // Display zeigt "Bereit"
          display.clearDisplay();
          display.setTextColor(WHITE);
          display.setTextSize(1);
          display.setCursor(18, 5);
          display.println("Reisealarmanlage");
          display.setTextSize(2);
          display.setCursor(34, 19);
          display.println("Alarm");
          display.setCursor(25, 40);
          display.setTextSize(2);
          display.println("Bereit!");
          display.display();
          display.invertDisplay(false);
        }
        else if(dauer <= 45){
          // Display zeigt QR-Code
          displayQRcodeAnzeigen();
          display.display();

        }
        else dauer = 0;
        dauer++;

        // (Sensorvariablen werden durch WebUI gesetzt)
        if (bIR_Sensor_an || bUS_Sensor_an || bA_Sensor_an) {
          ldistultraschallvgl = ReturnUltraschall();
          Zustand = Aktiv;
          bSensor_ausgeloest = 0;
        }
        break;

      case Aktiv:
        // Display zeigt "Aktiv"
        display.clearDisplay();
        display.setTextColor(WHITE);
        display.setTextSize(1);
        display.setCursor(18, 5);
        display.println("Reisealarmanlage");
        display.setTextSize(2);
        display.setCursor(34, 19);
        display.println("Alarm");
        display.setCursor(31, 40);
        display.setTextSize(2);
        display.println("AKTIV!");
        display.display();
        display.invertDisplay(false);

        //Infrarotsensor
        if(bIR_Sensor_an){
          //Sensor abfragen
          bSensor_ausgeloest = ReturnInfrarot();
          //drift korrigieren

        }
        //Ultraschallsensor
        if(bUS_Sensor_an){
          //Sensor abfragen & auswerten
          if ( ReturnUltraschall() - ldistultraschallvgl > 5 || ReturnUltraschall() - ldistultraschallvgl < -5 )  {
            bSensor_ausgeloest = 1;
          }
          //drift korrigieren
          if (ReturnUltraschall() < ldistultraschallvgl){
            ldistultraschallvgl--;
          }
          else ldistultraschallvgl++;
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
        //Falls entschärft wird
        if(!(bIR_Sensor_an||bUS_Sensor_an||bA_Sensor_an)){
          Zustand = Start;
          bSensor_ausgeloest = 0;
        }
        break;

      case Alarm:
        // LED blinken, Piezo aktivieren
        // Web UI kann bSensor_ausgeloest auf 0 setzen (z. B. später über extra Button)
        AlarmOutput(); 

        if (!bSensor_ausgeloest) {
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

/* =======================================================================
 *  Funktionen: Alarm und Sensoren
 * ======================================================================= */

/* LED blinken, Piezo aktivieren */
void AlarmOutput(){
  static unsigned int takt = 0;     //reset nach jedem Piepser
  //Serial.println(takt);
  
  if (dauer < 300 && takt < 4){
    digitalWrite(LED_PIN, 1);       // LED AN
    digitalWrite(PIEZZO_PIN, 1);    // PIEZZO AN
    //Serial.println("AN");             // Debug

    // Display zeigt "ALARM!"
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(14, 20);
    display.println("ALARM!");
    display.display();
    display.invertDisplay(true);
  }
  else if (dauer < 300 && takt < 6){
    digitalWrite(LED_PIN, 0);       // LED AUS    
    digitalWrite(PIEZZO_PIN, 0);    // PIEZZO AUS
    //Serial.println("AUS");            // Debug

    // Display zeigt "ALARM!"
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(14, 20);
    display.println("ALARM!");
    display.display();
    display.invertDisplay(false);
  }
  else if(takt < 4){                //Nach 300 Piepsern nur noch LED und Display
    digitalWrite(LED_PIN, 1);

    // Display zeigt "ALARM!"
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(14, 20);
    display.println("ALARM!");
    display.display();
    display.invertDisplay(true);
  }
  else if(takt < 6){
    digitalWrite(LED_PIN, 0);

    // Display zeigt "ALARM!"
    display.clearDisplay();
    display.setTextColor(WHITE);
    display.setTextSize(3);
    display.setCursor(14, 20);
    display.println("ALARM!");
    display.display();
    display.invertDisplay(false);
  }
  else{
    takt = 0;
    dauer++;
  }
  takt++;
}

/**
 * @brief Gibt die gemessene Entfernung in cm zurück
*/
long ReturnUltraschall()  {
  //Drift berücksichtigen!!!
  return ultrasonic.MeasureInCentimeters(/*TimeOUT in microsek */ );   // Für Initialisierung
}

/**
 * @brief Gibt Status von IR_Sensor zurück
*/
bool ReturnInfrarot() {
  return digitalRead(INFRAROT_PIN_NR);
}

/**
 * @brief Ließt Beschleunigungssensor aus
 * Rückgabe:  1 bei Bewegung erkannt,  0 wenn keine Bewegung erkannt
 *
 * @TODO: Sensitivität kontrollieren & anpassen?!
*/
bool ReturnAcceleration() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(acc_x, acc_y, acc_z);

    if (acc_x - acc_x_old > SENSI || acc_x - acc_x_old < -SENSI) {
      //Serial.print("X:");
      //Serial.println(acc_x, 5);   //Debug-Text
      acc_x_old = acc_x;
      return 1;
    }
    if (acc_y - acc_y_old > SENSI || acc_y - acc_y_old < -SENSI) {
      //Serial.print("Y:");
      //Serial.println(acc_y, 5);   //Debug-Text
      acc_y_old = acc_y;
      return 1;
    }
    if (acc_z - acc_z_old > SENSI || acc_z - acc_z_old < -SENSI) {
      //Serial.print("Z:");
      //Serial.println(acc_z, 5);   //Debug-Text
      acc_z_old = acc_z;
      return 1;
    }
  }
  else Serial.println("WTF");

  return 0;
}

/* =======================================================================
 *  WEB: Access Point + HTTP-Handler
 * ======================================================================= */

void WebServer_begin() {

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi-Modul nicht erkannt!");
    while (true);
  }

  Serial.println("Starte Access Point...");
  int status = WiFi.beginAP(ssid, pass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Fehler beim Starten des Access Points.");
    while (true);
  }

  delay(5000);

  IPAddress ip = WiFi.localIP();
  Serial.println("Access Point aktiv!");
  Serial.print("IP Adresse: ");
  Serial.println(ip);

  server.begin();
}

void WebServer_handleClient() {
  WiFiClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (c == '\n') {
          Serial.println("----- HTTP Request -----");
          Serial.println(request);
          Serial.println("------------------------");

          if (request.indexOf("GET /status") >= 0) {
            Serial.println("Status abgefragt");
            sendStatus(client);
          } 
          else {
            if (request.indexOf("GET /start?") >= 0) {
              ueberwachungAktiv = true;

              bIR_Sensor_an =   request.indexOf("pir=1")   >= 0;
              bUS_Sensor_an =   request.indexOf("ultra=1") >= 0;
              bA_Sensor_an  =   request.indexOf("acc=1")   >= 0;

              Serial.println("=== Überwachung gestartet ===");
              Serial.print("PIR: ");   Serial.println(bIR_Sensor_an);
              Serial.print("Ultra: "); Serial.println(bUS_Sensor_an);
              Serial.print("Acc: ");   Serial.println(bA_Sensor_an);
              Serial.println("================================");
            }
            else if (request.indexOf("GET /stop") >= 0) {
              ueberwachungAktiv = false;

              bIR_Sensor_an = false;
              bUS_Sensor_an = false;
              bA_Sensor_an  = false;
              bSensor_ausgeloest = false;   // Alarm zurücksetzen

              Serial.println("=== Überwachung gestoppt ===");
            }

            sendWebApp(client);
          }

          break;
        }
      }
    }
    client.stop();
  }
}

/* JSON-Status für den Browser */
void sendStatus(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  String json = "{";
  json += "\"ueberwachung\":"; json += (ueberwachungAktiv ? "true" : "false");
  json += ",\"pir\":";         json += (bIR_Sensor_an    ? "true" : "false");
  json += ",\"ultra\":";       json += (bUS_Sensor_an    ? "true" : "false");
  json += ",\"acc\":";         json += (bA_Sensor_an     ? "true" : "false");
  json += "}";

  client.println(json);
}

/* Web-App senden */
void sendWebApp(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close");
  client.println();

  client.println(R"rawliteral(
<!DOCTYPE html>
<html lang="de">
<head>
  <meta charset="UTF-8">
  <title>Reisealarmanlage</title>
  <style>
    body { 
      font-family: Arial; 
      background:#f5f7fa; 
      margin:0; 
      padding:20px; 
      text-align:center;
    }
    h1 { color:#222; }
    .sensor-box {
      background:white;
      padding:20px;
      max-width:400px;
      margin:20px auto;
      border-radius:10px;
      box-shadow:0 0 10px rgba(0,0,0,0.1);
      text-align:left;
    }
    .sensor-box label {
      font-size:18px;
      display:block;
      margin:10px 0;
    }
    button {
      font-size:18px;
      padding:12px 25px;
      margin:10px;
      border:none;
      border-radius:8px;
      cursor:pointer;
    }
    .start {
      background:#28a745; 
      color:white;
    }
    .stop {
      background:#dc3545; 
      color:white;
    }
    .status {
      font-size:20px;
      font-weight:bold;
      margin-top:15px;
    }
  </style>
</head>
<body>

<h1>Reisealarmanlage</h1>

<div class="sensor-box">
  <label><input type="checkbox" id="pir"> PIR-Sensor</label>
  <label><input type="checkbox" id="ultra"> Ultraschallsensor</label>
  <label><input type="checkbox" id="acc"> Beschleunigungssensor</label>
</div>

<button class="start" onclick="start()">Überwachung starten</button>
<button class="stop" onclick="stop()">Überwachung stoppen</button>

<div class="status" id="statusText">INAKTIV</div>

<script>
async function ladeStatus() {
  try {
    const res = await fetch('/status');
    if (!res.ok) return;
    const data = await res.json();

    // Checkboxen anhand des Arduino-Status setzen
    document.getElementById("pir").checked   = !!data.pir;
    document.getElementById("ultra").checked = !!data.ultra;
    document.getElementById("acc").checked   = !!data.acc;

    // Textstatus aktualisieren
    document.getElementById("statusText").innerText =
      data.ueberwachung ? "AKTIV" : "INAKTIV";
  } catch (e) {
    console.error("Status konnte nicht geladen werden:", e);
  }
}

async function start() {
  let params = [];

  if (document.getElementById("pir").checked)   params.push("pir=1");
  if (document.getElementById("ultra").checked) params.push("ultra=1");
  if (document.getElementById("acc").checked)   params.push("acc=1");

  let query = params.join("&");

  await fetch('/start?' + query);
  // danach Status nochmal vom Arduino holen
  ladeStatus();
}

async function stop() {
  await fetch('/stop');
  ladeStatus();
}

// Beim Laden der Seite aktuellen Zustand vom Arduino holen
window.addEventListener('load', ladeStatus);
</script>

</body>
</html>
)rawliteral");
}

void displayQRcodeAnzeigen(){
  
  display.clearDisplay();
  display.drawBitmap(0, 0, qrCode, 49, 49, WHITE);  //51, 19 ;; 25, 25

}