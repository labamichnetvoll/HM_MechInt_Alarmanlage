// WebServer.ino
#include "WebServer.h"
#include <SPI.h>
#include <WiFiNINA.h>

// ------- WLAN-Access-Point Daten -------
char apSsid[] = "ReiseAlarmanlage";
char apPass[] = "HalloFlo:)";      // min. 8 Zeichen für WPA2

WiFiServer server(80);

// ---- Verbindung zu deinem Zustandsautomaten ----
// Achtung: Enum und Variable müssen exakt so auch in der INO-Datei existieren!

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

extern Zustaende Zustand;   // kommt aus deiner .ino-Datei

// Status-Variablen für Webausgabe
String webModus = "HOTEL";
bool   webUeberwachungAktiv = false;
bool   webAlarm = false;
String webGeraeteStatus = "MODUS HOTEL";

// ------------------------------------------------------------------
// Hilfsfunktion: Web-Status aus dem Automaten ableiten
// ------------------------------------------------------------------
static void updateWebStatusFromZustand() {
  webUeberwachungAktiv = false;
  webAlarm = false;

  switch (Zustand) {
    case Auswahl_Hotel:
      webModus = "HOTEL";
      webGeraeteStatus = "MODUS HOTEL";
      break;
    case Auswahl_Gepaeck:
      webModus = "GEPÄCK";
      webGeraeteStatus = "MODUS GEPÄCK";
      break;
    case Auswahl_Zelt:
      webModus = "ZELT";
      webGeraeteStatus = "MODUS ZELT";
      break;

    case Scharf_Hotel:
      webModus = "HOTEL";
      webGeraeteStatus = "Hotel AKTIV";
      webUeberwachungAktiv = true;
      break;
    case Scharf_Gepaeck:
      webModus = "GEPÄCK";
      webGeraeteStatus = "Gepäck AKTIV";
      webUeberwachungAktiv = true;
      break;
    case Scharf_Zelt:
      webModus = "ZELT";
      webGeraeteStatus = "Zelt AKTIV";
      webUeberwachungAktiv = true;
      break;

    case Alarm:
      webGeraeteStatus = "ALARM!!!";
      webUeberwachungAktiv = true;
      webAlarm = true;
      // webModus bleibt einfach auf dem letzten Wert
      break;

    default:
      webGeraeteStatus = "UNBEKANNT";
      break;
  }
}

// ------------------------------------------------------------------
// HTML-Seite senden (hier stark gekürzt – du kannst deine gesamte
// HTML-Seite einfügen)
// ------------------------------------------------------------------
static void sendHtmlPage(WiFiClient &client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html; charset=utf-8");
  client.println("Connection: close");
  client.println();

  client.println(F("<!DOCTYPE html>"));
  client.println(F("<html lang=\"de\"><head><meta charset=\"UTF-8\">"));
  client.println(F("<title>Reisealarmanlage Steuerung</title>"));
  client.println(F("<style>"
    "body{font-family:Arial,sans-serif;background-color:#f0f4ff;padding:20px;text-align:center;}"
    "h1{color:#2c3e50;}"
    ".status{margin:20px 0;font-size:18px;}"
    "select,button{padding:10px 20px;font-size:16px;margin:10px;border-radius:6px;}"
    ".alarm{background-color:red;color:white;padding:15px;font-weight:bold;display:none;border-radius:8px;margin-top:20px;}"
    "</style></head><body>"));

  client.println(F("<h1>Reisealarmanlage</h1>"
    "<div class=\"status\">"
    "<div><strong>Gerätestatus:</strong> <span id=\"geraetStatus\">MODUS HOTEL</span></div>"
    "<div><strong>Aktueller Modus:</strong> <span id=\"modusStatus\">HOTEL</span></div>"
    "<div><strong>Überwachung:</strong> <span id=\"ueberwachungStatus\">INAKTIV</span></div>"
    "</div>"));

  client.println(F("<label for=\"modus\">Modus wählen:</label>"
    "<select id=\"modus\" onchange=\"modusWechseln()\">"
    "<option value=\"hotel\">HOTEL</option>"
    "<option value=\"gepaeck\">GEPÄCK</option>"
    "<option value=\"zelt\">ZELT</option>"
    "</select>"));

  client.println(F("<div>"
    "<button onclick=\"starteUeberwachung()\">🔒 Starte Überwachung</button>"
    "<button onclick=\"stoppeUeberwachung()\">🔓 Beende Überwachung</button>"
    "</div>"));

  client.println(F("<div id=\"alarmBox\" class=\"alarm\">🚨 ALARM!!!</div>"));

  client.println(F("<script>"
    "async function modusWechseln(){"
      "const modus=document.getElementById('modus').value;"
      "try{await fetch('/modus?typ='+modus);ladeStatus();}catch(e){console.error(e);}"
    "}"
    "async function starteUeberwachung(){"
      "try{await fetch('/start');ladeStatus();}catch(e){console.error(e);}"
    "}"
    "async function stoppeUeberwachung(){"
      "try{await fetch('/stop');ladeStatus();}catch(e){console.error(e);}"
    "}"
    "async function ladeStatus(){"
      "try{const res=await fetch('/status');if(!res.ok)return;"
      "const data=await res.json();"
      "document.getElementById('geraetStatus').innerText=data.geraet;"
      "document.getElementById('modusStatus').innerText=data.modus;"
      "document.getElementById('ueberwachungStatus').innerText=data.ueberwachung;"
      "const select=document.getElementById('modus');"
      "let m=data.modus.toLowerCase();"
      "m=m.replace('ä','ae');"
      "if(m==='hotel'||m==='gepaeck'||m==='zelt'){select.value=m;}"
      "document.getElementById('alarmBox').style.display=data.alarm?'block':'none';"
      "}catch(e){console.error('Status konnte nicht geladen werden:',e);}"
    "}"
    "setInterval(ladeStatus,1000);"
    "ladeStatus();"
    "</script>"));

  client.println(F("</body></html>"));
}

// ------------------------------------------------------------------
// JSON für /status senden
// ------------------------------------------------------------------
static void sendStatusJson(WiFiClient &client) {
  updateWebStatusFromZustand();

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: application/json; charset=utf-8");
  client.println("Connection: close");
  client.println();

  client.print("{\"geraet\":\"");
  client.print(webGeraeteStatus);
  client.print("\",\"modus\":\"");
  client.print(webModus);
  client.print("\",\"ueberwachung\":\"");
  client.print(webUeberwachungAktiv ? "AKTIV" : "INAKTIV");
  client.print("\",\"alarm\":");
  client.print(webAlarm ? "true" : "false");
  client.println("}");
}

// ------------------------------------------------------------------
// Öffentliche Funktionen
// ------------------------------------------------------------------
void WebServer_begin() {
  Serial.println("Starte Access Point...");

  int status = WiFi.beginAP(apSsid, apPass);
  if (status != WL_AP_LISTENING) {
    Serial.println("Fehler beim Starten des AP!");
  } else {
    Serial.println("AP gestartet.");
    Serial.print("SSID: ");
    Serial.println(apSsid);
    Serial.print("IP: ");
    Serial.println(WiFi.localIP()); // meist 192.168.4.1
  }

  server.begin();
  Serial.println("HTTP-Server läuft auf Port 80.");
}

void WebServer_handleClient() {
  WiFiClient client = server.available();
  if (!client) return;

  String reqLine = client.readStringUntil('\r');  // erste Zeile "GET /... HTTP/1.1"
  while (client.available()) client.read();       // Rest der Header wegwerfen

  Serial.print("HTTP-Request: ");
  Serial.println(reqLine);

  // Root: HTML-Seite
  if (reqLine.indexOf("GET / ") >= 0) {
    sendHtmlPage(client);
  }
  // Moduswechsel
  else if (reqLine.indexOf("GET /modus?typ=hotel") >= 0) {
    Zustand = Auswahl_Hotel;
    updateWebStatusFromZustand();
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  else if (reqLine.indexOf("GET /modus?typ=gepaeck") >= 0) {
    Zustand = Auswahl_Gepaeck;
    updateWebStatusFromZustand();
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  else if (reqLine.indexOf("GET /modus?typ=zelt") >= 0) {
    Zustand = Auswahl_Zelt;
    updateWebStatusFromZustand();
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  // Überwachung starten
  else if (reqLine.indexOf("GET /start") >= 0) {
    // hier kannst du noch Countdown etc. in deinem Automaten ausführen lassen
    switch (Zustand) {
      case Auswahl_Hotel:   Zustand = Scharf_Hotel;   break;
      case Auswahl_Gepaeck: Zustand = Scharf_Gepaeck; break;
      case Auswahl_Zelt:    Zustand = Scharf_Zelt;    break;
      default: break;
    }
    updateWebStatusFromZustand();
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  // Überwachung stoppen
  else if (reqLine.indexOf("GET /stop") >= 0) {
    // laut Programmlogik: zurück in Default-Modus Hotel
    Zustand = Auswahl_Hotel;
    updateWebStatusFromZustand();
    client.println("HTTP/1.1 200 OK");
    client.println("Connection: close");
    client.println();
  }
  // Status als JSON
  else if (reqLine.indexOf("GET /status") >= 0) {
    sendStatusJson(client);
  }
  // Unbekannt
  else {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
  }

  delay(1);
  client.stop();
}
