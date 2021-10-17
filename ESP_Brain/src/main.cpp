#include <Arduino.h>

#include "LoopTasks.h"
#include "SingleTasks.h"
//#include "ota.h"

#define SERVO_DELAY_SECONDS 3

// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};
volatile MaximumData maxData;

Queue queue;
Timer_ms frameTimer;

// Odcięcie przy ciśnieniu w butli poniżej 36 barów (aby nie doszło do zassania gazów spalinowych i eksplozji butli):
bool safetyCutoff_36atm = true;
uint32_t liftoffTime;

/**********************************************************************************************/

void setup() {

    mainDataFrame.rocketState = INIT;

    Serial.begin(115200);
    delay(500);

    //initOtaSerwer();
    //if (useOta) serverOta.begin();

    valveInit();

    xTaskCreate(i2cTask,                "Task i2c",     65536,  NULL, 3, NULL);
    xTaskCreate(sdTask,                 "Task SD",      65536, NULL, 2, NULL);
    xTaskCreate(adcTask,                "Task ADC",     4096,  NULL, 1, NULL);
    xTaskCreate(thrustControllerTask,   "Task TC",      16384,  NULL, 1, NULL);
    
    if(!nowInit())
        mainDataFrame.espNowErrorCounter = 2137; // Fatalny błąd.

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    saveFrameHeaders();

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
 *   5. Sterowanie silnikiem zaworu upustowego, [DONE]
 *   6. Obsługa maszyny stanów,                 [DONE]
 *   1. Pitot - ESP now,                        [DONE]
 *   2. Główny zawór - ESP now.                 [DONE]
 */

void loop() {
    uart2Handler();

    // Otworzenie zaworu upustowego w razie braku łączności przez długi czas:
    if (mainDataFrame.abortTimerSec < 1) {
        xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
    }

    //if (useOta) serverOta.handleClient();

    if (mainDataFrame.rocketState == IDLE) {

        frameTimer.setVal(WAIT_DATA_PERIOD*5);

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie IDLE.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
            mainDataFrame.abortTimerSec--;
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FUELING) {

        frameTimer.setVal(WAIT_DATA_PERIOD*2);

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie FUELING.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
            mainDataFrame.abortTimerSec--;
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == COUNTDOWN) {

        frameTimer.setVal(1000);


        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie COUNTDOWN.

            mainDataFrame.countdown--;

            if(mainDataFrame.countdown == SERVO_DELAY_SECONDS) {

                // Odpal silnik:
                vTaskDelay(400 / portTICK_PERIOD_MS);
                Serial.println("Zapalnik");
                Serial2.print("TNWN;DSTA\n");
                vTaskDelay(10 / portTICK_PERIOD_MS);
            }

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if(mainDataFrame.countdown < 1) {

                // Przyspiesz pomiary z pitota:
                uint16_t pitotPeriod = 50;
                if(esp_now_send(adressPitot, (uint8_t *) &pitotPeriod, sizeof(pitotPeriod)))
                    mainDataFrame.espNowErrorCounter++;

                // Każ serwu się otworzyć:
                Serial.println("Serwo");
                char messageOpen[] = "MNVL;1";
                if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
                    mainDataFrame.espNowErrorCounter++;

                liftoffTime = millis();
                mainDataFrame.rocketState = FLIGHT;
            }
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == ABORT) {

        frameTimer.setVal(END_DATA_PERIOD);

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie ABORT.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FLIGHT) {

        frameTimer.setVal(FLIGHT_DATA_PERIOD);

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie FLIGHT.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if (mainDataFrame.separationData & (1<<1)) {
                
                mainDataFrame.rocketState = FIRST_SEPAR;
                stateChanger.flight2firstSepar();
            }

            // Odcięcie bezpieczeństwa (udawane):
            else if (safetyCutoff_36atm && mainDataFrame.tankPressure < 36.0F) {
                
                safetyCutoff_36atm = false;
                queue.push(String("R4MC;Ponizej 36 barow\n"));
            }

            // Odcięcie symulacji (udawane):
            else if (closeValveRequest && millis() - liftoffTime > 2500) {

                closeValveRequest = false;
                queue.push(String("R4MC;Wystarczy do 3km\n"));
            }
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FIRST_SEPAR) {

        frameTimer.setVal(FLIGHT_DATA_PERIOD*10);

        if (frameTimer.check()) {

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);

            if (mainDataFrame.separationData & (1<<2)) {

                mainDataFrame.rocketState = SECOND_SEPAR;
                stateChanger.firstSep2secSep();
            }
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == SECOND_SEPAR) {

        frameTimer.setVal(WAIT_DATA_PERIOD*2);

        if (frameTimer.check()) {

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