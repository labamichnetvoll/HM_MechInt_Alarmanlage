#include "Sensorik.h"

#define PERIOD_UPDATE 100   // Taktlänge in ms

long lastupdate = 0;
enum Zustand {
  Start,
  Initialisierung,
  Auswahl_Hotel,
  Auswahl_Zelt,
  Auswahl_Gepaeck,
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
      break;

    case Initialisierung:
      break;

    case Auswahl_Hotel:
      break;

    case Auswahl_Zelt:
      break;

    case Auswahl_Gepaeck:
      break;

    case Scharf_Hotel:
      break;

    case Scharf_Gepaeck:
      break;

    case Scharf_Zelt:
      break;
    
    case Alarm:
      break;

    default Default:
      Zustand = Start;
      break;
    }
  }
}

// Funktionen hier

