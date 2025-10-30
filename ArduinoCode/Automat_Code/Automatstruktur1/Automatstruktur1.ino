#define PERIOD_UPDATE 100   // Zeit in ms

long lastupdate = millis();
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
  Zustand = Start;

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lastupdate > PERIOD_UPDATE)  {
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

    case Auswahl_Gepaek:
      break;

    case Scharf_Hotel:
      break;

    case Scharf_Gepaek:
      break;

    case Scharf_Zelt:
      break;
    
    case Alarm:
      break;
      
    }
    lastupdate = millis();
  }

}

// Funktionen hier

