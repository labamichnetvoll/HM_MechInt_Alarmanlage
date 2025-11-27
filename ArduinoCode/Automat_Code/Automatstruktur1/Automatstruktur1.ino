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
//#include "WebServer.h"           // Web Anteil - aktuell nicht benutzt

/* PIN Nummer - Defintionen*/
#define ULTRASONIC_PIN_NR 7     // evtl. anpassen! TODO   
#define INFRAROT_PIN_NR 6       // evtl. anpassen! TODO
#define BUTTON_PIN 2            // evtl. anpassen! TODO     Auswahl Pins Interrupt: 2, 3, 9, 10, 11, 13, A1, A5, A7
#define LED_PIN 4               // evtl. anpassen! TODO
#define PIEZZO_PIN 5            // evtl. anpassen! TODO
#define PWM_PIN 9               // evtl. anpassen! TODO     Wichtig PWM PIN erwischen
#define SHDN_PIN 10             // evtl. anpassen! TODO     Verstärker AN/AUS
#define MUTE_PIN 11             // evtl. anpassen! TODO     Verstärker Mute  

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

// Webserver / WLAN
const char ssid[] = "ReiseAlarm_AP";     // Name des WLANs
const char pass[] = "12345678";          // Passwort (mind. 8 Zeichen)

WiFiServer server(80);

// Zustand nur für Web-Anzeige (Start/Stop)
bool ueberwachungAktiv = false;

/* Konstruktor CPP Klassen*/
Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);

/* Funktionsprototypen*/
void AlarmOutput();
void WebServer_begin();
void WebServer_handleClient();
void sendStatus(WiFiClient client);
void sendWebApp(WiFiClient client);

// Sensor-Funktionen – hier als Prototypen, unten implementiert:
long ReturnUltraschall();
bool ReturnInfrarot();
bool ReturnAcceleration();

/* =======================================================================
 *  SETUP
 * ======================================================================= */
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
    Serial.println("Ultraschall defekt (Distanz = 0)");
  }

  /* Automat-Init*/
  Zustand = Start;        // Zustand auf Start
  ulLastupdate = millis();  // Taktzähler zurücksetzen

  // Webserver starten (Access Point + HTTP-Server)
  WebServer_begin();
}

/* =======================================================================
 *  LOOP
 * ======================================================================= */
void loop() {

  // HTTP-Anfragen vom Web-Client bearbeiten
  WebServer_handleClient();
  
  if (millis() - ulLastupdate > PERIOD_UPDATE)  {   // if-Abfrage funktioniert für mindestens 7 Wochen, dann Overflow möglich

    ulLastupdate = millis();  // Taktzähler zurücksetzen

    ReturnAcceleration();   // TEST

    //Zustandswechsel hier
    switch(Zustand){
      
      case Start:
        // Alarm ausschalten
        analogWrite(PWM_PIN, 0);
        digitalWrite(PWM_PIN, 0); // safety first
        digitalWrite(SHDN_PIN, 0);      // Verstärker AUS

        //Sensoren und Aktoren Initialisieren
        bIR_Sensor_an = 0;
        bUS_Sensor_an = 0;
        bA_Sensor_an = 0;
        bSensor_ausgeloest = 0;

        Zustand = Sensorauswahl;
        break;
      
      case Sensorauswahl:
        // Display zeigt "Bereit"
        // (Sensorvariablen werden durch WebUI gesetzt)
        if (bIR_Sensor_an || bUS_Sensor_an || bA_Sensor_an) {
          ldistultraschallvgl = ReturnUltraschall();
          Zustand = Aktiv;
        }
        break;

      case Aktiv:
        if (bIR_Sensor_an) {
          // Sensor abfragen
          bSensor_ausgeloest = ReturnInfrarot();
        }
        if (bUS_Sensor_an) {
          // Sensor abfragen & auswerten
          long dist = ReturnUltraschall();
          if (dist - ldistultraschallvgl > 5 || dist - ldistultraschallvgl < -5)  {
            bSensor_ausgeloest = 1;
          }
        }
        if (bA_Sensor_an) {
          // Sensor abfragen
          bSensor_ausgeloest = ReturnAcceleration();
        }

        if (bSensor_ausgeloest) {
          Zustand = Alarm;
        }
        break;

      case Alarm:
        // LED blinken, Lautsprecher aktivieren, Piezo aktivieren
        // Web UI kann bSensor_ausgeloest auf 0 setzen (z. B. später über extra Button)
        AlarmOutput();

        if (!bSensor_ausgeloest) {
          Zustand = Start;
        }
        break;

      default:
        // Nur zur Fehlerbehebung, immer zu Start wechseln
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

// LED blinken, Lautsprecher aktivieren, Piezo aktivieren
void AlarmOutput(){

  static unsigned long takt = 0;
  digitalWrite(SHDN_PIN, 1);      // Verstärker AN

  if (takt % 2) digitalWrite(PIEZZO_PIN, 1);   // PIEZZO AN
  else          digitalWrite(PIEZZO_PIN, 0);   // PIEZZO AUS

  analogWrite(PWM_PIN, 50);      // 0-255: mitte am lautesten
 
  if (takt < 20){
    digitalWrite(LED_PIN, 1);    // LED AN
  }
  else if (takt < 40){
    digitalWrite(LED_PIN, 0);    // LED AUS    
  }
  else {
    takt = 0;
  }
  
  takt++;
}

// Ultraschall – simple Beispielimplementierung
long ReturnUltraschall() {
  long d = ultrasonic.read();   // hängt von Bibliothek ab
  // Serial.print("Ultraschall: "); Serial.println(d);
  return d;
}

// Infrarot-Sensor – HIGH = ausgelöst
bool ReturnInfrarot() {
  int val = digitalRead(INFRAROT_PIN_NR);
  return (val == HIGH);
}

// Beschleunigungssensor – Schwellwert-Beispiel
bool ReturnAcceleration() {
  float x, y, z;
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z);
    // einfache Schwelle
    if (fabs(x) > 0.5 || fabs(y) > 0.5 || fabs(z) > 0.5) {
      return true;
    }
  }
  return false;
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
