#include <Arduino.h>


#include "LoopTasks.h"
#include "SingleTasks.h"

#define SERVO_DELAY 100

// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

/* Ta zmienna przyjmuje wartość true, gdy trwa zapis do mainDataFrame.
Jest po to aby inne taski czekały aż ten zapis się skończy, by w tym czasie nie odczytywać głównej struktury: */
volatile bool mainDataFrameSaveBusy = false; 

Queue queue;
Timer_ms frameTimer;
volatile bool forceStateAction;

/**********************************************************************************************/

void setup() {

    mainDataFrame.rocketState = INIT;

    Serial.begin(115200);
    delay(100);

    valveInit();

    xTaskCreate(i2cTask,    "Task i2c",     65536,  NULL, 3, NULL);
    xTaskCreate(sdTask,     "Task SD",      65536, NULL, 1, NULL);
    xTaskCreate(adcTask,    "Task ADC",     4096,  NULL, 1, NULL);
    
    if(!nowInit())
        mainDataFrame.espNowErrorCounter = 2137; // Fatalny błąd.

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    mainDataFrame.rocketState = IDLE;
    Serial2.begin(115200);
    Serial2.setTimeout(1);
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej oraz ESP now. Obsługuje:
 *   1. Stan Gpsa - do zapisu na SD,            [DONE]
 *   2. Stan Tanwy - do zapisu na SD,           [DONE]
 *   3. Polecenia przychodzące z LoRy,          [DONE]
 *   4. Ramki, które chcemy wysłać do LoRy,     [DONE]
 *   5. Sterowanie silnikiem zaworu upustowego, [ALMOST_DONE]
 *   6. Obsługa maszyny stanów,                 [TODO]
 *   1. Pitot - ESP now,                        [TODO - Done earlier]
 *   2. Główny zawór - ESP now.                 [ALMOSt_DONE]
 */

void loop() {
    uart2Handler();

    if (mainDataFrame.rocketState == IDLE) {
        
        frameTimer.setVal(WAIT_DATA_PERIOD*5);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FUELING) {
    
        frameTimer.setVal(WAIT_DATA_PERIOD*2);
        if (forceStateAction) {

            forceStateAction = false;

            // Każ serwu zamknąć zawór:
            char messageOpen[] = "MNVL;0";
            if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
                mainDataFrame.espNowErrorCounter++;

            // TODO trzeba gdzieś jeszcze dorobić ruchy zaworem upustowym - może w funkcji czytającej z UARTu
        }

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == COUNTDOWN) {
        
       frameTimer.setVal(WAIT_DATA_PERIOD);


        if (frameTimer.check()) {

            mainDataFrame.countdown--;

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if(mainDataFrame.countdown < 1) {

                // Przyspiesz pomiary z pitota:
                uint16_t pitotPeriod = 50;
                if(esp_now_send(adressPitot, (uint8_t *) &pitotPeriod, sizeof(pitotPeriod)))
                    mainDataFrame.espNowErrorCounter++;

                // Odpal silnik:
                Serial2.print("Lecimy!\n");

                vTaskDelay(SERVO_DELAY / portTICK_PERIOD_MS);

                // Każ serwu się otworzyć:
                char messageOpen[] = "MNVL;1";
                if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
                    mainDataFrame.espNowErrorCounter++;

                mainDataFrame.rocketState = FLIGHT;
            }
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == ABORT) {
        
        frameTimer.setVal(END_DATA_PERIOD);
        if (forceStateAction) {

            forceStateAction = false;
            // otworzyć zawór upustowy.
        }

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FLIGHT) {
        
       frameTimer.setVal(FLIGHT_DATA_PERIOD);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if (mainDataFrame.separationData & (1<<1))
                mainDataFrame.rocketState = FIRST_SEPAR;
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FIRST_SEPAR) {
        
        frameTimer.setVal(FLIGHT_DATA_PERIOD*10);

        if (frameTimer.check()) {

            // Spowolnij pomiary z pitota:
            uint16_t pitotPeriod = 400;
            if(esp_now_send(adressPitot, (uint8_t *) &pitotPeriod, sizeof(pitotPeriod)))
                mainDataFrame.espNowErrorCounter++;

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if (mainDataFrame.separationData & (1<<2))
                mainDataFrame.rocketState = SECOND_SEPAR;
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == SECOND_SEPAR) {
        
        frameTimer.setVal(WAIT_DATA_PERIOD*2);

        if (frameTimer.check()) {

            // Spowolnij jeszcze mocniej pomiary z pitota:
            uint16_t pitotPeriod = 8000;
            if(esp_now_send(adressPitot, (uint8_t *) &pitotPeriod, sizeof(pitotPeriod)))
                mainDataFrame.espNowErrorCounter++;

            // Otworzenie zaworów, wyłączenie silników i serw.
            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == GROUND) {
        
        frameTimer.setVal(END_DATA_PERIOD);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    vTaskDelay(2 / portTICK_PERIOD_MS);
}