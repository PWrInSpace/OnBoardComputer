#include "Tasks.h"

void i2cTask(void *arg) {

    Wire.begin();

    while(1) {

        for (int i = 0; i < 5; i++) {
        
            Wire.requestFrom(3, 1);
            delay(10);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val);
            }
            delay(500);
        }

        Wire.beginTransmission(3);
        Wire.write(8);
        Wire.endTransmission();
        delay(5);

        for (;;) {
            
            Wire.requestFrom(3, 1);
            delay(10);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val);
            }
            delay(500);
        }
    }
}