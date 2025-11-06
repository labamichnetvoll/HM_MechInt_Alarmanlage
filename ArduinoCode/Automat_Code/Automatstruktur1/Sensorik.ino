/**
 * eigene Sensorik "Bibliothek" für die einfachere Anwendungen einzelner Sensoren
 * soll zur Übersichtlichkeit beitragen.
 * 
 * https://docs.arduino.cc/tutorials/nano-33-iot/imu-accelerometer/
*/


float acc_x, acc_y, acc_z;    //Speicher für Beschleunigungswerte
int degreesX = 0;
int degreesY = 0;
int degreesZ = 0;      

/**
 * @brief Gibt die gemessene Entfernung in cm zurück
*/
long ReturnUltraschall()  {
  //Drift berücksichtigen!!!
  return ultrasonic.MeasureInCentimeters();   //

}


/**
 * @brief Gibt Status von IR_Sensor zurück
*/
int ReturnInfrarot(int pin_nr) {
  return digitalRead(pin_nr);
}


/**
 * @brief Ließt Beschleunigungssensor aus
 * Rückgabewert: bool ( Erfolgreich gelesen ) 0: nein, 1: Erfolg
 *
 * TODO: evtl mit Parametern aufrufen und über Pointer werte Übergeben. alternativ nur Ausgelöst ja/nein?
*/
int ReadIMUAcc() {
  bool dataread;
  if (IMU.accelerationAvailable()) {
    dataread = IMU.readAcceleration(acc_x, acc_y, acc_z);

  }
  else{
    return 0;
  }

/* Test Code --> IMU Verifizieren*/
  if (acc_x > 0.1) {
    acc_x = 100 * acc_x;
    degreesX = map(acc_x, 0, 97, 0, 90);
    Serial.print("Tilting up ");
    Serial.print(degreesX);
    Serial.println("  degrees");
  }
  if (acc_x < -0.1) {
    acc_x = 100 * acc_x;
    degreesX = map(acc_x, 0, -100, 0, 90);
    Serial.print("Tilting down ");
    Serial.print(degreesX);
    Serial.println("  degrees");
  }
  if (acc_y > 0.1) {
    acc_y = 100 * acc_y;
    degreesY = map(acc_y, 0, 97, 0, 90);
    Serial.print("Tilting left ");
    Serial.print(degreesY);
    Serial.println("  degrees");
  }
  if (acc_y < -0.1) {
    acc_y = 100 * acc_y;
    degreesY = map(acc_y, 0, -100, 0, 90);
    Serial.print("Tilting right ");
    Serial.print(degreesY);
    Serial.println("  degrees");
  }
  if (acc_z > 0.1) {
    acc_z = 100 * acc_z;
    degreesZ = map(acc_z, 0, 97, 0, 90);
    Serial.print("Tilting front? ");
    Serial.print(degreesZ);
    Serial.println("  degrees");
  }
  if (acc_z < -0.1) {
    acc_z = 100 * acc_z;
    degreesZ = map(acc_z, 0, -100, 0, 90);
    Serial.print("Tilting back? ");
    Serial.print(degreesZ);
    Serial.println("  degrees");
  }

  return dataread;
}


