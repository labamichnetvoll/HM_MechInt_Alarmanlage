/**
 * eigene Sensorik "Bibliothek" für die einfachere Anwendungen einzelner Sensoren
 * soll zur Übersichtlichkeit beitragen.
 * 
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
*/

#define SENSI 0.01

float acc_x, acc_y, acc_z;    //Speicher für Beschleunigungswerte
float acc_x_old, acc_y_old, acc_z_old;  
  

/**
 * @brief Gibt die gemessene Entfernung in cm zurück
*/
long ReturnUltraschall()  {
  //Drift berücksichtigen!!!
  return ultrasonic.MeasureInCentimeters(/*TimeOUT in microsek */ );   // Für Initialisierung

}


/**
 * @brief Gibt Status von IR_Sensor zurück
*/
bool ReturnInfrarot() {
  return digitalRead(INFRAROT_PIN_NR);
}


/**
 * @brief Ließt Beschleunigungssensor aus
 * Rückgabe:  1 bei Bewegung erkannt,  0 wenn keine Bewegung erkannt
 *
 * @TODO: Sensitivität kontrollieren & anpassen?!
*/
bool ReturnAcceleration() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(acc_x, acc_y, acc_z);

    if (acc_x - acc_x_old > SENSI || acc_x - acc_x_old < -SENSI) {
      //Serial.print("X:");
      //Serial.println(acc_x, 5);
      acc_x_old = acc_x;
      return 1;
    }
    if (acc_y - acc_y_old > SENSI || acc_y - acc_y_old < -SENSI) {
      //Serial.print("Y:");
      //Serial.println(acc_y, 5);
      acc_y_old = acc_y;
      return 1;
    }
    if (acc_z - acc_z_old > SENSI || acc_z - acc_z_old < -SENSI) {
      //Serial.print("Z:");
      //Serial.println(acc_z, 5);
      acc_z_old = acc_z;
      return 1;
    }

  }
  else{ }

  return 0;
}


