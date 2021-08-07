#include "Tasks.h"

// Zadanie zajmujące się wszystkim, co łączy się przez i2c:

void i2cTask(void *arg) {

    Wire.begin();

    while(1) {

        for (int i = 0; i < 5; i++) {
        
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val); // Tylko do debugu
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }

        Wire.beginTransmission(3);
        Wire.write(8);
        Wire.endTransmission();
        vTaskDelay(5 / portTICK_PERIOD_MS);

        for (;;) {
            
            Wire.requestFrom(3, 1);
            vTaskDelay(10 / portTICK_PERIOD_MS);
            if (Wire.available()) {
                int val = Wire.read();
                Serial.println(val); // Tylko do debugu
            }
            vTaskDelay(500 / portTICK_PERIOD_MS);
        }
    }
}