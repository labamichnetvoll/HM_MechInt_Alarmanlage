// WebServer.h
#pragma once
#include <Arduino.h>

// vom Hauptprogramm aufrufen:
void WebServer_begin();        // AP starten + Server starten
void WebServer_handleClient(); // in loop() regelmäßig aufrufen
