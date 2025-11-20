#include <SPI.h>
#include <WiFiNINA.h>

const char ssid[] = "ReiseAlarm_AP";     // Name des WLANs
const char pass[] = "12345678";          // Passwort (mind. 8 Zeichen)

WiFiServer server(80);

// Zustände
bool ueberwachungAktiv = false;

// Zustände der Sensoren
bool sensorPIR = false;
bool sensorUltra = false;
bool sensorAcc = false;

void setup() {
  Serial.begin(9600);
  while (!Serial);

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

          // START
          if (request.indexOf("GET /start?") >= 0) {
            ueberwachungAktiv = true;

            sensorPIR   = request.indexOf("pir=1") >= 0;
            sensorUltra = request.indexOf("ultra=1") >= 0;
            sensorAcc   = request.indexOf("acc=1") >= 0;

            Serial.println("=== Überwachung gestartet ===");
            Serial.print("PIR: "); Serial.println(sensorPIR);
            Serial.print("Ultraschall: "); Serial.println(sensorUltra);
            Serial.print("Beschleunigung: "); Serial.println(sensorAcc);
            Serial.println("================================");
          }
          // STOP
          else if (request.indexOf("GET /stop") >= 0) {
            ueberwachungAktiv = false;

            Serial.println("=== Überwachung gestoppt ===");
            sensorPIR = sensorUltra = sensorAcc = false;
          }

          // Webseite senden
          sendWebApp(client);
          break;
        }
      }
    }
    client.stop();
  }

  // --- HIER WÜRDEST DU SPÄTER SENSORLOGIK EINBAUEN ---
  if (ueberwachungAktiv) {
    if (sensorPIR) {
      // PIR auslesen
    }
    if (sensorUltra) {
      // Ultraschall auslesen
    }
    if (sensorAcc) {
      // Beschleunigungssensor auslesen
    }
  }
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
function start() {
  let params = [];

  if (document.getElementById("pir").checked) params.push("pir=1");
  if (document.getElementById("ultra").checked) params.push("ultra=1");
  if (document.getElementById("acc").checked) params.push("acc=1");

  let query = params.join("&");

  fetch('/start?' + query);
  document.getElementById("statusText").innerText = "AKTIV";
}

function stop() {
  fetch('/stop');
  document.getElementById("statusText").innerText = "INAKTIV";
}
</script>

</body>
</html>
)rawliteral");
}
