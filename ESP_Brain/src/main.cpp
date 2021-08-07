#include <Arduino.h>
#include <Wire.h>

#include "now.h"

MainDataFrame mainDataFrame = {};
States state;

void setup(){
  
    Serial.begin(115200);

    /*nowInit();

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);*/

    Wire.begin();
    Serial.println("wire jest");
}

void loop() {

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    Serial.println("for1");
    for (int i = 0; i < 5; i++) {
        
        Wire.requestFrom(3, 1);
        delay(100);
        if (Wire.available()) {
            int val = Wire.read();
            Serial.println(val);
        }
        delay(1000);
    }

    Serial.println("Leci arm");
    Wire.beginTransmission(3);
    Wire.write(8);
    Wire.endTransmission();
    delay(10);
  
    Serial.println("for2");
    for (;;) {
        
        Wire.requestFrom(3, 1);
        delay(100);
        if (Wire.available()) {
            int val = Wire.read();
            Serial.println(val);
        }
        delay(1000);
    }
}