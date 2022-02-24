#include "LoopTasks.h"

extern DataFrame dataFrame;
extern File file;
extern QueueHandle_t queue;

/* Zadanie odpowiedzialne za logowanie wszystkiego na Flashu -> pobierając dane z kolejki.
 */

void flashTask(void *arg) {

    while (1) {

        if (uxQueueMessagesWaiting(queue) > 10) {

            DataFrame data;

            file = LITTLEFS.open("/file.txt", "a");
            
            while (uxQueueMessagesWaiting(queue) > 0) {

                xQueueReceive(queue, &data, portMAX_DELAY);
                file.write((uint8_t*) &data, sizeof(data));
            }
            
            file.close();
        }

        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}

/**********************************************************************************************/

/* Funkcja odpowiedzialna za pomiary analogowe.
 */

void adcMeasure() {

    EspBinData measureData;

    // Ciśnienie butli:
    measureData.tankPressure = analogRead(GPIO_NUM_34) * 0.0201465 * 1.11; // * 82.5 / 4095
    /* 
        * Mnożymy przez 82.5, bo przy takim ciśnieniu będzie maksymalne napięcie na adc - 3.3v
        * (Czujnik daje 10v przy 250 barach).
        * dzielimy przez 4095 bo to max wartość z przetwornika
        */

    // Akumulator:
    measureData.batteryV = analogRead(GPIO_NUM_36) / 254.0 * 0.893;

    Serial2.printf("ESP;%f;%f", measureData.batteryV, measureData.tankPressure);
    Serial.printf("ESP;%f;%f\n", measureData.batteryV, measureData.tankPressure);
}