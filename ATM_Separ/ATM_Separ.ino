  
#include "Globals.h"
#include "Wire.h"

void setup() {

  delay(1000);
  initSeparationSystem(); // Ustaw piny itp.

}

/*******************************************************/

void loop() {

  separationMonitoringLoop(); // Monitoruj zachowanie Altimaxa i telemetrum.
}