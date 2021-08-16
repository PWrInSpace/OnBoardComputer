#include <Arduino.h>


#include "LoopTasks.h"
#include "SingleTasks.h"
#include "Timer_ms.h"


// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

/* Ta zmienna przyjmuje wartość true, gdy trwa zapis do mainDataFrame.
Jest po to aby inne taski czekały aż ten zapis się skończy, by w tym czasie nie odczytywać głównej struktury: */
volatile bool mainDataFrameSaveBusy = false; 

Queue queue;
Timer_ms frameTimer;

/**********************************************************************************************/

void setup() {

    mainDataFrame.rocketState = INIT;

    Serial.begin(115200);
    delay(100);

    valveInit();

    xTaskCreate(i2cTask,    "Task i2c",     8192,  NULL, 1, NULL);
    xTaskCreate(sdTask,     "Task SD",      65536, NULL, 1, NULL);
    xTaskCreate(adcTask,    "Task ADC",     4096,  NULL, 1, NULL);
    
    if(!nowInit()) {

        mainDataFrame.espNowErrorCounter++;
    }

    //nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    mainDataFrame.rocketState = IDLE;
    Serial2.begin(115200);
    Serial2.setTimeout(1);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej oraz ESP now. Obsługuje:
 *   1. Stan Gpsa - do zapisu na SD,            [ALMOST_DONE]
 *   2. Stan Tanwy - do zapisu na SD,           [ALMOST_DONE]
 *   3. Polecenia przychodzące z LoRy,          [TODO]
 *   4. Ramki, które chcemy wysłać do LoRy,     [TODO]
 *   5. Sterowanie silnikiem zaworu upustowego, [TODO]
 *   6. Obsługa maszyny stanów,                 [TODO]
 *   1. Pitot - ESP now,                        [TODO - Done earlier]
 *   2. Główny zawór - ESP now.                 [TODO]
 */

void loop() {
    uart2Handler();

    if (mainDataFrame.rocketState == IDLE) {
        
        frameTimer.setVal(5000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == FUELING) {
        
        frameTimer.setVal(2000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == COUNTDOWN) {
        
       frameTimer.setVal(1000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == ABORT) {
        
        frameTimer.setVal(10000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == FLIGHT) {
        
       frameTimer.setVal(50);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == FIRST_SEPAR) {
        
        frameTimer.setVal(500);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == SECOND_SEPAR) {
        
        frameTimer.setVal(2000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    else if (mainDataFrame.rocketState == GROUND) {
        
        frameTimer.setVal(10000);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    vTaskDelay(2 / portTICK_PERIOD_MS);
    

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    /*while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    queue.push("R4GP;" + String(millis()));
    queue.push("R4TN;" + String(millis()));
    queue.push("R4MC;" + String(millis()));
    Serial.println(String(millis()));
    vTaskDelay(1000 / portTICK_PERIOD_MS);*/
}