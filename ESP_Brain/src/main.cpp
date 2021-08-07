#include <Arduino.h>

#include "Tasks.h"

volatile MainDataFrame mainDataFrame = {};
bool mainDataFrameSaveBusy = false;

States state = INIT;

void setup(){
  
    Serial.begin(115200);

    xTaskCreate(i2cTask,        "Task i2c",     10000, NULL, 1, NULL);
    xTaskCreate(uart3AntsTask,  "Uart 3-ant",   10000, NULL, 1, NULL);
    xTaskCreate(sdTask,         "Task SD",      50000, NULL, 1, NULL);
    xTaskCreate(espNowTask,     "Task Esp Now", 10000, NULL, 1, NULL);
}

void loop() {

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/
}