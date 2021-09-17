  
#include "Globals.h"
#include "Wire.h"

void setup() {

  delay(10);
  initSeparationSystem(); // Ustaw piny itp.

}

/*******************************************************/

void loop() {

  separationMonitoringLoop(); // Monitoruj zachowanie Altimaxa i telemetrum.
  continuityMonitoringLoop(); // Monitoruj ciągłość.
}
