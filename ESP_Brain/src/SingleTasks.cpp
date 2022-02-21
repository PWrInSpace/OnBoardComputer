#include "SingleTasks.h"

extern DataFrame dataFrame;

int upustTime_ms;

void uart2Handler() {

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        vTaskDelay(2 / portTICK_PERIOD_MS);
        Serial2.readBytes((uint8_t*) &dataFrame, sizeof(dataFrame));

    // TODO Dodanie danych do kolejki do zapisu we flashu
    }
}