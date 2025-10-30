#define PERIOD_UPDATE 100   // Zeit in ms

long lastupdate = millis();
enum Zustand {
  Start,
  Initialisierung,
  Auswahl_Hotel,
  Auswahl_Zeit,
  Auswahl_Gepaeck,
  Scharf_Hotel,
  Scharf_Gepaeck,
  Scharf_Zelt,
  Alarm
}



void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lastupdate > PERIOD_UPDATE)  {
    //Zustandswechsel hier


    lastupdate = millis();
  }



}

// Funktionen hier

