#include "SingleTasks.h"

extern DataFrame dataFrame;
extern QueueHandle_t queue;

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        vTaskDelay(2 / portTICK_PERIOD_MS);
        Serial2.readBytes((uint8_t*) &dataFrame, sizeof(dataFrame));

        xQueueSend(queue, &dataFrame, portMAX_DELAY);
    }
}