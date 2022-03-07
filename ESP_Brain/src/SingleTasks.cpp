#include "SingleTasks.h"

extern DataFrame dataFrame;
extern File file;
extern QueueHandle_t queue;

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        vTaskDelay(2 / portTICK_PERIOD_MS);
        Serial2.readBytes((uint8_t*) &dataFrame, sizeof(dataFrame));

        xQueueSend(queue, &dataFrame, 100);
    }

    // Komendy z kompa:
    if (Serial.available()) {

        String command = Serial.readString();

        if (strstr(command.c_str(), "ReadFlash")) {
            file = LITTLEFS.open("/file.txt");

            Serial.println("Flash:");

            while (file.available()) {

                file.readBytes((char*) &dataFrame, sizeof(dataFrame));

                Serial.printf("R4OR;%d;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d\n",
                    (int)dataFrame.time_ms, 	dataFrame.batteryV, 		dataFrame.tankPressure,
                    dataFrame.gpsLatitude, 		dataFrame.gpsLongitude, 	dataFrame.gpsAltitude,
                    dataFrame.gpsSatNum, 		dataFrame.gpsTimeS,			dataFrame.hallSensors[0],
                    dataFrame.hallSensors[1], 	dataFrame.hallSensors[2],	dataFrame.hallSensors[3],
                    dataFrame.hallSensors[4], 	dataFrame.tanWaVoltage,		dataFrame.tanWaState,
                    dataFrame.tanWaIgniter,		dataFrame.tanWaFill,		dataFrame.tanWaDepr,
                    dataFrame.tanWaUpust, 		dataFrame.rocketWeightKg, 	dataFrame.tankWeightKg,
                    (int)dataFrame.rocketWRaw, 	(int)dataFrame.tankWRaw);
            }
            
            file.close();
        }
    }
}