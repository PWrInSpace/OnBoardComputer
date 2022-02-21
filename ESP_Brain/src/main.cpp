#include <Arduino.h>

#include "LoopTasks.h"
#include "SingleTasks.h"

// Główna struktura na wszelnie dane z rakiety:
volatile DataFrame dataFrame = {};
File file;
QueueHandle_t queue;
Timer_ms frameTimer;

/**********************************************************************************************/

void setup() {

    delay(100);

    Serial.begin(115200);
    LITTLEFS.begin(true);

    queue = xQueueCreate(20, sizeof(DataFrame));
    xTaskCreate(flashTask, "Task save to Flash", 8192, NULL, 1, NULL);

    Serial2.begin(115200);
    Serial2.setTimeout(10);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej oraz ESP now. Obsługuje:
 *   1. Ogarnianie zapisu danych do flasha,
 *   2. Odczytu danych z flasha,
 *   3. Pomiaru i wysyłania ADC,
 */

void loop() {

    uart2Handler();

    adcMeasure();

    vTaskDelay(2 / portTICK_PERIOD_MS);
}