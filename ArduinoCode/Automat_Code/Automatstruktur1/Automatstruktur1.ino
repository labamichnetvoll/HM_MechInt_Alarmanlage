#define PERIOD_UPDATE 100   // Taktlänge in ms

long lastupdate = 0;
enum Zustand {
  Start,
  Initialisierung,
  Auswahl_Hotel,
  Auswahl_Gepaeck,
  Auswahl_Zelt,
  Scharf_Hotel,
  Scharf_Gepaeck,
  Scharf_Zelt,
  Alarm
}


void setup() {

  Zustand = Start;        //Zustand auf Start
  lastupdate = millis();  //Taktzähler zurücksetzen

}


void loop() {

  if (millis() - lastupdate > PERIOD_UPDATE)  {

    lastupdate = millis();  //Taktzähler zurücksetzen

    //Zustandswechsel hier
    switch(Zustand){

    case Start:
      Zustand = Initialisierung;
      break;

    case Initialisierung:
      //IR-Sensor initialisieren
      //a-Sensor initialisieren
      //Schallsensor initialisieren
      //Display initialisieren
      //ISR initialisieren
      Zustand = Auswahl_Hotel;  //Nach Abschluss: zu Auswahl_Hotel wechseln
      break;

    case Auswahl_Hotel:
      //Anzeige: Hotelmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Gepaeck wechseln
      //Falls langer Knopfdruck: Countdown, dann zu Scharf_Hotel wechseln
      break;

    case Auswahl_Gepaeck:
      //Anzeige: Gepäckmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Zelt wechseln
      //Falls langer Knopfdruck: Countdowhn, dann zu Scharf_Gepaeck wechseln
      break;

    case Auswahl_Zelt:
      //Anzeige: Zeltmodus
      //Falls kurzer Knopfdruck: zu Auswahl_Hotel wechseln
      //Falls langer Knopfdruck: Countdown, dann zu Scharf_Zelt wechseln
      break;

    case Scharf_Hotel:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //Schallsensor abfragen
      //Falls Abstand stark verändert: zu Alarm wechseln
      break;

    case Scharf_Gepaeck:
     //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //a-Sensor abfragen
      //Falls starke Beschleunigung: zu Alarm wechseln
      break;

    case Scharf_Zelt:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //IR-Sensor abfragen
      //Falls Bewegung erkannt: zu Alarm wechseln
      break;
    
    case Alarm:
      //Entschärfung prüfen
      //Falls entschärft: zu Initialisierung wechseln
      //Sonst: LED blinken, Lautsprecher aktivieren, Piezo aktivieren
      break;

    default Default:
      //Nur zur Fehlerbehebung, immer zu Start wechseln
      Zustand = Start;
      break;
    }
  }
}

// Funktionen hier

