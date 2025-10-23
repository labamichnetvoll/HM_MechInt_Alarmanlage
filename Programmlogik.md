# Programmlogik 

---------------------------------------------------------
## GERÄT ÜBER HAUPTSCHALTER ANSCHALTEN

Wenn Wippschalter == 1
   dann Powerbank auf high schalten 
   und am Display "MODUS HOTEL" anzeigen          // Modus "Hotel" als Default-Zustand
   und in Web-App "MODUS HOTEL" anzeigen

---------------------------------------------------------
## MODUSWECHSELN
// Moduswechsel über Drucktaster oder Web-App möglich

Wenn Drucktaster 1x kurz betätigt         // max. 1 Sekunde
   dann Wechsel zwischen Modi:
      Modus "Gepäck"             // scannt Bewegungen über Beschleunigungssensor auf dem Arduino
         am Display "MODUS GEPÄCK" anzeigen        
         und in Web-App "MODUS GEPÄCK" anzeigen
      Modus "Zelt"               // scannt Eindringen in Zelt über IR-Bewegungssensor
         am Display "MODUS ZELT" anzeigen        
         und in Web-App "MODUS ZELT" anzeigen
      Modus "Hotel"              // scannt anvisierte Tür/Fenster über Ultraschallsensor
         am Display "MODUS HOTEL" anzeigen        
         und in Web-App "MODUS HOTEL" anzeigen

ODER

Wenn Auswahl in Web-App == Modus "Gepäck"
   am Display "MODUS GEPÄCK" anzeigen        
   und in Web-App "MODUS GEPÄCK" anzeigen
Wenn Auswahl in Web-App == Modus "Zelt"          
   am Display "MODUS ZELT" anzeigen        
   und in Web-App "MODUS ZELT" anzeigen
Wenn Auswahl in Web-App == Modus "Hotel"         
   am Display "MODUS HOTEL" anzeigen        
   und in Web-App "MODUS HOTEL" anzeigen

---------------------------------------------------------
## ÜBERWACHUNG AKTIVIEREN
// Aktivieren über Drucktaster oder Web-App möglich

Wenn Drucktaster 1x lang betätigt         // mind. 3 Sekunden
oder 
Wenn "Starte Überwachung" in Web-App gewählt
   dann am Display "Starte Überwachung" anzeigen
   und in Web-App "Starte Überwachung" anzeigen

   warte 10 Sekunden
   
      wenn Modus "Hotel" == 1
          dann aktiviere Ultraschall
          und am Display "Hotel AKTIV" anzeigen
          und in Web-App "Hotel AKTIV" anzeigen
      wenn Modus "Gepäck" == 1
          dann aktiviere Beschleunigungssensor
          und am Display "Gepäck AKTIV" anzeigen
          und in Web-App "Gepäck AKTIV" anzeigen
      wenn Modus "Zelt" == 1
          dann aktiviere IR-Sensor
          und am Display "Zelt AKTIV" anzeigen
          und in Web-App "Zelt AKTIV" anzeigen

---------------------------------------------------------
## ÜBERWACHUNG DEAKTIVIEREN
// Deaktivieren über Web-App oder Strom kappen am Hauptschalter 

Wenn "Beende Überwachung" in Web-App gewählt
   dann am Display "BEENDE ÜBERWACHUNG" anzeigen
   und in Web-App "BEENDE ÜBERWACHUNG" anzeigen 

   warte 5 Sekunden

   Wenn vorher "Starte Überwachung" gewählt
   break;

   Sonst deaktiviere alle Sensoren
   und am Display "MODUS HOTEL" anzeigen        
   und in Web-App "MODUS HOTEL" anzeigen
       
---------------------------------------------------------
## ALARMZUSTAND

Wenn einer der Sensor auslöst        
   dann LED an
   und Lautsprecher an
   und Buzzer an
   und am Display "ALARM!!!" anzeigen
   und in Web-App "ALARM!!!" anzeigen

---------------------------------------------------------
## GERÄT ÜBER HAUPTSCHALTER AUSSCHALTEN

Wenn Wippschalter == 0                // wichtig: hardwareseitig Schalter "verstecken", um Manipulation zu erschweren
   dann Powerbank auf low schalten
   und in Web-App "AUS" anzeigen
