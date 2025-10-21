# Programmlogik 

---------------------------------------------------------
## GERÄT ÜBER HAUPTSCHALTER ANSCHALTEN

Wenn Wippschalter == 1
   dann Powerbank auf high schalten 
   und in Web-App "MODUS WÄHLEN" anzeigen

---------------------------------------------------------
## MODUSWECHSELN

Wenn Drucktaster 1x betätigt       // entspricht Modus "Raum"
   dann in Web-App "RAUMCHECK" anzeigen

Wenn Drucktaster 2x betätigt       // entspricht Modus "Gepäck" 
   dann in Web-App "GEPÄCKCHECK" anzeigen

Wenn Drucktaster 3x betätigt      // entspricht Modus "Modus wählen"
   dann in Web-App "MODUS WÄHLEN" anzeigen

---------------------------------------------------------
## ALARM AKTIV SCHALTEN

Wenn Überwachung aktiv == 1             // in Web-App schalten
   dann in Web-App "STARTE ÜBERWACHUNG" anzeigen
   und warte 5 Sekunden
      wenn Modus "GEPÄCKCHECK" == 1
          dann aktiviere Beschleunigungssensor
          und in Web-App "GEPÄCKCHECK AKTIV" anzeigen
      wenn Modus "RAUMCHECK" == 1
          dann aktiviere Bewegungssensor
          und in Web-App "RAUMCHECK AKTIV" anzeigen

---------------------------------------------------------
## ALARMMODUS DEAKTIVIEREN

Wenn Überwachung aktiv == 0           // in Web-App schalten
    dann in Web-App "BEENDE ÜBERWACHUNG" anzeigen
    und warte 5 Sekunden
       wenn Überwachung aktiv == 1
          break;
       wenn Überwachug aktiv == 0
          dann deaktiviere alle Sensoren
          und in Web-App "MODUS WÄHLEN" anzeigen
       
---------------------------------------------------------
## ALARMZUSTAND

Wenn Sensor ausgelöst        
   dann LED an
   und Lautsprecher an
   und Buzzer an
   und in Web-App "ALARM!!!" anzeigen

---------------------------------------------------------
## GERÄT ÜBER HAUPTSCHALTER AUSSCHALTEN

Wenn Wippschalter == 0                // wichtig: hardwareseitig Schalter "verstecken", um Manipulation zu erschweren
   dann Powerbank auf low schalten
   und in Web-App "AUS" anzeigen
