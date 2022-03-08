#include "SingleTasks.h"

extern DataFrame dataFrame;
extern File file;
extern QueueHandle_t queue;
extern QueueHandle_t sdQueue;

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        vTaskDelay(2 / portTICK_PERIOD_MS);
        Serial2.readBytes((uint8_t*) &dataFrame, sizeof(dataFrame));

        xQueueSend(queue, &dataFrame, 100);
        xQueueSend(sdQueue, &dataFrame, 100);
    }

    // Komendy z kompa:
    if (Serial.available()) {

        String command = Serial.readString();

        if (strstr(command.c_str(), "ReadFlash")) {
            file = LITTLEFS.open("/file.txt");

            Serial.println("Flash:");

            while (file.available()) {

                file.readBytes((char*) &dataFrame, sizeof(dataFrame));

                Serial.print(data2String(dataFrame));
            }
            
            file.close();
        }
    }
}