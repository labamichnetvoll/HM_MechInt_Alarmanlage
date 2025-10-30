#define PERIOD_UPDATE 100   // Zeit in ms

long lastupdate = millis();




void setup() {
  

}

void loop() {
  // put your main code here, to run repeatedly:

  if (millis() - lastupdate > PERIOD_UPDATE)  {
    //Zustandswechsel hier


    lastupdate = millis();
  }



}

// Automaten Schritte hier

