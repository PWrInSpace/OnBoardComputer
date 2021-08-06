#include <Arduino.h>
#include <Wire.h>

#include "now.h"

MainDataFrame mainDataFrame = {};
States state;

void setup(){
  
    Serial.begin(115200);

    nowInit();

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    Wire.begin();
}

void loop() {

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    for(int i = 0; i < 5; i++) {
        
        Wire.requestFrom(3, 1);
        delay(100);
        while (Wire.available()) {
            int val = Wire.read();
            Serial.println(val);
        }
        delay(1000);
    }

    Wire.beginTransmission(3);
    Wire.write(8);
    Wire.endTransmission();
  
    for(;;) {
        
        Wire.requestFrom(3, 1);
        delay(100);
        while (Wire.available()) {
            int val = Wire.read();
            Serial.println(val);
        }
        delay(1000);
    }
}