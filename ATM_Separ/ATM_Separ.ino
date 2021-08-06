#include "Globals.h"

void setup() {

  delay(1000);
  initSeparationSystem(); // Ustaw piny itp.

}

/*******************************************************/

void loop() {

  i2cLoop(); // Czekaj na polecenia i2c dotyczące odbioru lub wysłania danych.
  separationMonitoringLoop(); // Monitoruj zachowanie Altimaxa i telemetrum.

}

/*******************************************************/

void initSeparationSystem() {

  
}

/*******************************************************/

void i2cLoop() {

  
}

/*******************************************************/

void separationMonitoringLoop() {

  
}
