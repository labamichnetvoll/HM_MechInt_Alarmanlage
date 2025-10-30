#include "api/Common.h"
#include "Ultrasonic.h"            //fertige Bibliothek für Ultraschall Sensor

#define ULTRASONIC_PIN_NR 7        // Pin Nummer 7
#define INFRAROT_PIN_NR 6           
Ultrasonic ultrasonic(ULTRASONIC_PIN_NR);


pinMode(INFRAROT_PIN_NR, INPUT);

long ReturnUltraschall(){
  //Drift berücksichtigen!!!
  return ultrasonic.MeasureInCentimeters();   //

}



boolean ReturnInfrarot(){

  return digitalRead(INFRAROT_PIN_NR);
}


//IMU dazu

