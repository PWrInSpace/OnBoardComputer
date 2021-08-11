#include <Arduino.h>


#include "LoopTasks.h"
#include "SingleTasks.h"


// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

/* Ta zmienna przyjmuje wartość true, gdy trwa zapis do mainDataFrame.
Jest po to aby inne taski czekały aż ten zapis się skończy, by w tym czasie nie odczytywać głównej struktury: */
volatile bool mainDataFrameSaveBusy = false; 

Queue queue;

States state = INIT;

void setup(){  
    Serial.begin(115200);
    delay(100);
    

    xTaskCreate(i2cTask,    "Task i2c",     4096,  NULL, 1, NULL);
    xTaskCreate(sdTask,     "Task SD",      32768, NULL, 1, NULL);
    xTaskCreate(espNowTask, "Task Esp Now", 65536, NULL, 1, NULL);
    xTaskCreate(adcTask,    "Task ADC",     4096,  NULL, 1, NULL);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej. Obsługuje:
 *   1. Stan Gpsa - do zapisu na SD,            [TODO]
 *   2. Stan Tanwy - do zapisu na SD,           [TODO]
 *   3. Polecenia przychodzące z LoRy,          [TODO]
 *   4. Ramki, które chcemy wysłać do LoRy,     [TODO]
 *   5. Sterowanie silnikiem zaworu upustowego, [TODO]
 *   6. Obsługa maszyny stanów.                 [TODO]
 */

void loop() {
    
    queue.push("R4GP;" + String(millis()));
    queue.push("R4TN;" + String(millis()));
    queue.push("R4MC;" + String(millis()));
    Serial.println(String(millis()));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}