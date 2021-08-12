#include <Arduino.h>


#include "LoopTasks.h"
#include "SingleTasks.h"


// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

/* Ta zmienna przyjmuje wartość true, gdy trwa zapis do mainDataFrame.
Jest po to aby inne taski czekały aż ten zapis się skończy, by w tym czasie nie odczytywać głównej struktury: */
volatile bool mainDataFrameSaveBusy = false; 

uint32_t mainLoopTimer;
uint16_t mainloopFreq[] = {
    5000,  //INIT,
    5000,  //IDLE,
    2000,  //FUELING,
    1000,  //COUNTDOWN,
    10000, //ABORT,
    50,  //FLIGHT,
    250,  //FIRST_SEPAR,
    500,  //SECOND_SEPAR,
    10000, //GROUND
};


Queue queue;

States state = INIT;

/**********************************************************************************************/

void setup(){  
    Serial.begin(115200);
    delay(100);

    xTaskCreate(i2cTask,    "Task i2c",     4096,  NULL, 1, NULL);
    xTaskCreate(sdTask,     "Task SD",      32768, NULL, 1, NULL);
    xTaskCreate(adcTask,    "Task ADC",     4096,  NULL, 1, NULL);

    if(!nowInit()) {

        mainDataFrame.espNowErrorCounter++;
    }

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    state = IDLE;
    mainLoopTimer = millis();
}

/**********************************************************************************************/

/* Zadanie odpowiedzialne za obsługę poleceń przychodzących po uartcie z płytki 3-antenowej oraz ESP now. Obsługuje:
 *   1. Stan Gpsa - do zapisu na SD,            [TODO]
 *   2. Stan Tanwy - do zapisu na SD,           [TODO]
 *   3. Polecenia przychodzące z LoRy,          [TODO]
 *   4. Ramki, które chcemy wysłać do LoRy,     [TODO]
 *   5. Sterowanie silnikiem zaworu upustowego, [TODO]
 *   6. Obsługa maszyny stanów,                 [TODO]
 *   1. Pitot - ESP now,                        [TODO - Done earlier]
 *   2. Główny zawór - ESP now.                 [TODO]
 */

void loop() {

    

    // Dane przychodzące z uartu:
    if (Serial2.available()) {

        vTaskDelay(2 / portTICK_PERIOD_MS);
        String dataFrom3Ant = Serial2.readString();
        
        if (dataFrom3Ant.indexOf("R4TN") == 0 || dataFrom3Ant.indexOf("R4GP") == 0) {
            queue.push(dataFrom3Ant);
        }

        //else if () TODO polecenia zaworu

        else if (dataFrom3Ant.indexOf("MNCP;STAT") == 0) {

            int oldState, newState;
            sscanf(dataFrom3Ant.c_str(), "MNCP;STAT;%d;%d", &oldState, &newState);

            if (oldState == (int) state) state = (States) newState;
        }

    }


    if (millis() - mainLoopTimer > mainloopFreq[(int) state]) {
    
    }

    /*char message[] = "wazna wiadomosc do przeslania\n";
    if(esp_now_send(adressPitot, (uint8_t *) message, strlen(message)))
        mainDataFrame.espNowErrorCounter++;*/

    while(1) {
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }

    queue.push("R4GP;" + String(millis()));
    queue.push("R4TN;" + String(millis()));
    queue.push("R4MC;" + String(millis()));
    Serial.println(String(millis()));
    vTaskDelay(1000 / portTICK_PERIOD_MS);
}