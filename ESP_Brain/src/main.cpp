#include <Arduino.h>

#include "now.h"
#include "Tasks.h"

volatile MainDataFrame mainDataFrame = {};
States state;

void setup(){
  
    Serial.begin(115200);

    /*nowInit();

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);*/

    xTaskCreate(i2cTask, "Task i2c", 10000, NULL, 1, NULL);
}

void loop() {

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/
}