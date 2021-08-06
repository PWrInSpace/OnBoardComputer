#include <Arduino.h>

#include "now.h"

MainDataFrame mainDataFrame = {};
States state;

void setup(){
  
  Serial.begin(115200);

  nowInit();

  nowAddPeer(adressPitot, 0);
  nowAddPeer(adressMValve, 0);
}

void loop() {

  /*char message[] = "wazna wiadomosc do przeslania\n";
  if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
    mainDataFrame.espNowErrorCounter++;*/

  delay(10);

}