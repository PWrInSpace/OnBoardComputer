#include <Arduino.h>

#include "Tasks.h"

// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

/* Ta zmienna przyjmuje wartość true, gdy trwa zapis do mainDataFrame.
Jest po to aby inne taski czekały aż ten zapis się skończy, by w tym czasie nie odczytywać głównej struktury: */
volatile bool mainDataFrameSaveBusy = false; 

States state = INIT;

void setup(){
  
    Serial.begin(115200);

    xTaskCreate(i2cTask,    "Task i2c",     10000, NULL, 1, NULL);
    xTaskCreate(sdTask,     "Task SD",      50000, NULL, 1, NULL);
    xTaskCreate(espNowTask, "Task Esp Now", 10000, NULL, 1, NULL);
    xTaskCreate(adcTask,    "Task ADC",     10000, NULL, 1, NULL);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej. Obsługuje:
 *   1. Odczyt gpsa z płytki 3-antenowej,       [TODO]
 *   2. Stan Tanwy,                             [TODO]
 *   3. Polecenia przychodzące z LoRy,          [TODO]
 *   4. Ramki, które chcemy wysłać do LoRy,     [TODO]
 *   5. Sterowanie silnikiem zaworu upustowego, [TODO]
 *   6. Obsługa maszyny stanów.                 [TODO]
 */

void loop() {

    
}