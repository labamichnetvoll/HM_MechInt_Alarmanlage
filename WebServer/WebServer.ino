#include <SPI.h>
#include <WiFiNINA.h>

const char ssid[] = "ReiseAlarm_AP";     // Name des WLANs
const char pass[] = "12345678";          // Passwort (mind. 8 Zeichen)

WiFiServer server(80);

// Zustände
String modus = "hotel";
bool ueberwachungAktiv = false;

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
          if (request.indexOf("GET /start?modus=") >= 0) {
            modus = getParam(request, "modus");
            ueberwachungAktiv = true;
            Serial.println("Überwachung gestartet. Modus: " + modus);
          }
          else if (request.indexOf("GET /stop") >= 0) {
            ueberwachungAktiv = false;
            Serial.println("Überwachung gestoppt");
          }
          else if (request.indexOf("GET /modus?typ=") >= 0) {
            modus = getParam(request, "typ");
            Serial.println("Modus geändert zu: " + modus);
          }

          sendWebApp(client);
          break;
        }
      }
    }
    client.stop();
  }
}

// Hilfsfunktion: Parameter aus HTTP-Request extrahieren
String getParam(String req, String key) {
  int start = req.indexOf(key + "=");
  if (start == -1) return "";
  start += key.length() + 1;
  int end = req.indexOf(" ", start);
  return req.substring(start, end);
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
  <title>Reisealarmanlage</title>
  <style>
    body { font-family: Arial; background-color:#eef; padding:20px; text-align:center; }
    h1 { color:#333; }
    select, button { font-size:16px; padding:10px 20px; margin:10px; }
    .status { font-size:18px; margin:20px 0; }
  </style>
</head>
<body>
  <h1>Reisealarmanlage</h1>

  <div class="status">
    <div><strong>Modus:</strong> <span id="modusStatus">-</span></div>
    <div><strong>Überwachung:</strong> <span id="ueberwachungStatus">-</span></div>
  </div>

  <label for="modus">Modus wählen:</label>
  <select id="modus" onchange="modusWechseln()">
    <option value="hotel">Hotel</option>
    <option value="gepaeck">Gepäck</option>
    <option value="zelt">Zelt</option>
  </select>

  <div>
    <button onclick="start()">🔒 Start</button>
    <button onclick="stop()">🔓 Stop</button>
  </div>

<script>
let modus = "hotel";
let ueberwachungAktiv = false;

function update() {
  document.getElementById("modusStatus").innerText = modus;
  document.getElementById("ueberwachungStatus").innerText = ueberwachungAktiv ? "AKTIV" : "INAKTIV";
}

function modusWechseln() {
  modus = document.getElementById("modus").value;
  fetch('/modus?typ=' + modus);
  update();
}

function start() {
  ueberwachungAktiv = true;
  fetch('/start?modus=' + modus);
  update();
}

function stop() {
  ueberwachungAktiv = false;
  fetch('/stop');
  update();
}

update();
</script>
</body>
</html>
)rawliteral");
}
