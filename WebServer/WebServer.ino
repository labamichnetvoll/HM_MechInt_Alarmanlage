#include <SPI.h>
#include <WiFiNINA.h>

const char ssid[] = "ReiseAlarm_AP";     // Name des WLANs
const char pass[] = "12345678";          // Passwort (mind. 8 Zeichen)

WiFiServer server(80);

// Zustände
String modus = "hotel";              // alter Modus, aktuell nicht mehr von der Weboberfläche benutzt
bool ueberwachungAktiv = false;      // true = Überwachung läuft

// Sensor- und Alarmzustände für die neue Weboberfläche
bool ultraschallAktiv = false;
bool beschleunigungAktiv = false;
bool irAktiv = false;
bool alarmAktiv = false;             // wird später von der Sensorlogik gesetzt
String geraetStatus = "BEREIT";      // einfacher Textstatus fürs JSON

void setup() {
  Serial.begin(9600);
  while (!Serial); // Warten auf USB-Verbindung

  // Prüfen, ob WiFi-Modul vorhanden ist
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

  delay(5000); // Kurze Wartezeit, bis AP bereit ist

  IPAddress ip = WiFi.localIP();
  Serial.println("Access Point aktiv!");
  Serial.print("IP Adresse: ");
  Serial.println(ip);

  server.begin();
}

void loop() {
  WiFiClient client = server.available();
  if (client) {
    String request = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        // Ende des HTTP-Headers
        if (c == '\n') {
          //hier werden die GET-Routen ausgewertet

          // Status-Abfrage von der Weboberfläche
          if (request.indexOf("GET /status") >= 0) {
            // JSON mit aktuellen Zuständen zurückschicken
            sendStatus(client);
          }
          // Start-Befehl mit Sensorparametern
          else if (request.indexOf("GET /start") >= 0) {
            // Parameter auslesen (0 oder 1)
            String u  = getParam(request, "ultraschall");
            String b  = getParam(request, "beschleunigung");
            String ir = getParam(request, "ir");

            ultraschallAktiv    = (u == "1");
            beschleunigungAktiv = (b == "1");
            irAktiv             = (ir == "1");

            ueberwachungAktiv = true;
            alarmAktiv        = false;        // beim Start Alarm zurücksetzen

            Serial.println("Überwachung gestartet");
            Serial.print("  Ultraschall: ");       Serial.println(ultraschallAktiv ? "AN" : "AUS");
            Serial.print("  Beschleunigung: ");    Serial.println(beschleunigungAktiv ? "AN" : "AUS");
            Serial.print("  IR: ");                Serial.println(irAktiv ? "AN" : "AUS");

            // einfache OK-Antwort, die Weboberfläche interessiert sich nur für den Status
            sendSimpleOk(client);
          }
          // Stop-Befehl
          else if (request.indexOf("GET /stop") >= 0) {
            ueberwachungAktiv    = false;
            ultraschallAktiv     = false;
            beschleunigungAktiv  = false;
            irAktiv              = false;
            alarmAktiv           = false;

            Serial.println("Überwachung gestoppt");

            sendSimpleOk(client);
          }
          // Alle anderen Anfragen bekommen die Weboberfläche (z.B. Aufruf im Browser)
          else {
            sendWebApp(client);
          }
          break;
        }
      }
    }
    client.stop();
  }
}

// Hilfsfunktion: Parameter aus HTTP-Request extrahieren
// Einfach gehalten, aber genug für die Query-Strings
String getParam(String req, String key) {
  int start = req.indexOf(key + "=");
  if (start == -1) return "";
  start += key.length() + 1;

  // Ende ist entweder ein '&' oder ein Leerzeichen (Ende der URL)
  int endAmp   = req.indexOf("&", start);
  int endSpace = req.indexOf(" ", start);

  int end = endSpace;
  if (endAmp != -1 && (endAmp < end || end == -1)) {
    end = endAmp;
  }

  if (end == -1) return "";

  return req.substring(start, end);
}

// Hilfsfunktion: JSON-Status für die Weboberfläche schicken
void sendStatus(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json");
  client.println("Connection: close");
  client.println();

  // einfache JSON zusammenbauen, reicht für unser Dashboard
  String json = "{";
  json += "\"geraet\":\"" + geraetStatus + "\"";
  json += ",\"ueberwachung\":\"" + String(ueberwachungAktiv ? "AKTIV" : "INAKTIV") + "\"";
  json += ",\"alarm\":" + String(alarmAktiv ? "true" : "false");
  json += ",\"ultraschall\":" + String(ultraschallAktiv ? 1 : 0);
  json += ",\"beschleunigung\":" + String(beschleunigungAktiv ? 1 : 0);
  json += ",\"ir\":" + String(irAktiv ? 1 : 0);
  json += "}";

  client.println(json);
}

// Einfache OK-Antwort für Start/Stop
void sendSimpleOk(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/plain");
  client.println("Connection: close");
  client.println();
  client.println("OK");
}


// Web-App senden
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
  <title>Reisealarmanlage Steuerung</title>
  <style>
    body {
      font-family: Arial, sans-serif;
      background-color: #f0f4ff;
      padding: 20px;
      text-align: center;
    }
    h1 { color: #2c3e50; }
    .status {
      margin: 20px 0;
      font-size: 18px;
    }
    button, label {
      font-size: 16px;
    }
    button {
      padding: 10px 20px;
      margin: 10px;
      border-radius: 6px;
    }
    .sensor-group {
      margin: 20px 0;
      font-size: 16px;
    }
    .sensor-group label {
      display: block;
      margin: 5px 0;
    }
    .alarm {
      background-color: red;
      color: white;
      padding: 15px;
      font-weight: bold;
      display: none;
      border-radius: 8px;
      margin-top: 20px;
    }
  </style>
</head>
<body>

  <h1>Reisealarmanlage</h1>

  <!-- Statusanzeigen -->
  <div class="status">
    <div><strong>Gerätestatus:</strong> <span id="geraetStatus">BEREIT</span></div>
    <div><strong>Aktive Sensoren:</strong> <span id="sensorStatus">KEINE</span></div>
    <div><strong>Überwachung:</strong> <span id="ueberwachungStatus">INAKTIV</span></div>
  </div>

  <!-- Sensorauswahl -->
  <div class="sensor-group">
    <strong>Sensoren auswählen:</strong>
    <label>
      <input type="checkbox" id="sensor_ultraschall">
      Ultraschallsensor
    </label>
    <label>
      <input type="checkbox" id="sensor_beschleunigung">
      Beschleunigungssensor
    </label>
    <label>
      <input type="checkbox" id="sensor_ir">
      IR-Sensor
    </label>
  </div>

  <!-- Überwachungssteuerung -->
  <div>
    <button onclick="starteUeberwachung()">Starte Überwachung</button>
    <button onclick="stoppeUeberwachung()">Beende Überwachung</button>
  </div>

  <!-- Alarmanzeige -->
  <div id="alarmBox" class="alarm">🚨 ALARM!!!</div>

  <script>
    async function starteUeberwachung() {
      // Ausgewählte Sensoren auslesen
      const ultraschall    = document.getElementById("sensor_ultraschall").checked ? 1 : 0;
      const beschleunigung = document.getElementById("sensor_beschleunigung").checked ? 1 : 0;
      const ir             = document.getElementById("sensor_ir").checked ? 1 : 0;

      // Wenn gar kein Sensor gewählt ist, abbrechen
      if (ultraschall === 0 && beschleunigung === 0 && ir === 0) {
        alert("Bitte mindestens einen Sensor auswählen.");
        return;
      }

      const url = `/start?ultraschall=${ultraschall}&beschleunigung=${beschleunigung}&ir=${ir}`;

      try {
        await fetch(url);
        ladeStatus();
      } catch (e) {
        console.error(e);
      }
    }

    async function stoppeUeberwachung() {
      try {
        await fetch("/stop");
        ladeStatus();
      } catch (e) {
        console.error(e);
      }
    }

    async function ladeStatus() {
      try {
        const res = await fetch("/status");
        if (!res.ok) return;
        const data = await res.json();

        document.getElementById("geraetStatus").innerText       = data.geraet       || "-";
        document.getElementById("ueberwachungStatus").innerText = data.ueberwachung || "-";

        // Sensor-Status aus JSON übernehmen (0/1 oder true/false)
        const ultraschallAktiv    = !!data.ultraschall;
        const beschleunigungAktiv = !!data.beschleunigung;
        const irAktiv             = !!data.ir;

        document.getElementById("sensor_ultraschall").checked    = ultraschallAktiv;
        document.getElementById("sensor_beschleunigung").checked = beschleunigungAktiv;
        document.getElementById("sensor_ir").checked             = irAktiv;

        const aktive = [];
        if (ultraschallAktiv)    aktive.push("Ultraschallsensor");
        if (beschleunigungAktiv) aktive.push("Beschleunigungssensor");
        if (irAktiv)             aktive.push("IR-Sensor");

        document.getElementById("sensorStatus").innerText =
          aktive.length ? aktive.join(", ") : "KEINE";

        // Alarmanzeige
        document.getElementById("alarmBox").style.display =
          data.alarm ? "block" : "none";

      } catch (e) {
        console.error("Status konnte nicht geladen werden:", e);
      }
    }

    // Alle 1 s Status vom Arduino holen
    setInterval(ladeStatus, 1000);
    // Beim Laden der Seite einmalig Status holen
    ladeStatus();
  </script>
</body>
</html>
)rawliteral");
}
