#include <Arduino.h>


#include "LoopTasks.h"
#include "SingleTasks.h"

#define SERVO_DELAY 100

// Główna struktura na wszelnie dane z rakiety:
volatile MainDataFrame mainDataFrame = {};

Queue queue;
Timer_ms frameTimer;
volatile bool forceStateAction;

// Odcięcie przy ciśnieniu w butli poniżej 36 barów (aby nie doszło do zassania gazów spalinowych i eksplozji butli):
bool safetyCutoff_36atm = false;

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
 *   5. Sterowanie silnikiem zaworu upustowego, [TESTING]
 *   6. Obsługa maszyny stanów,                 [DONE]
 *   1. Pitot - ESP now,                        [TESTING]
 *   2. Główny zawór - ESP now.                 [DONE]
 */

void loop() {
    uart2Handler();

    if (mainDataFrame.rocketState == IDLE) {
        
        frameTimer.setVal(WAIT_DATA_PERIOD*5);

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie IDLE.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == FUELING) {
    
        frameTimer.setVal(WAIT_DATA_PERIOD*2);
        if (forceStateAction) { // Jednorazowa akcja wykonywana tylko podczas przełączenia na FUELING.

            forceStateAction = false;

            // Każ serwu zamknąć zawór:
            char messageOpen[] = "MNVL;0";
            if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
                mainDataFrame.espNowErrorCounter++;

            // Zamknij także upustowy:
            xTaskCreate(valveClose, "Task close valve", 4096, NULL, 2, NULL);
        }

        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie FUELING.

            String txData = countStructData();
            queue.push(txData);
            sendData(txData);
        }
    }

    /*------------------------------------*/

    else if (mainDataFrame.rocketState == COUNTDOWN) {
        
       frameTimer.setVal(WAIT_DATA_PERIOD);


        if (frameTimer.check()) { // Polecenia wykonywane cyklicznie w stanie COUNTDOWN.

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
        if (forceStateAction) { // Jednorazowa akcja wykonywana tylko podczas przełączenia na ABORT.

            forceStateAction = false;
            // Otworzenie upustowego:
            xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
        }

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

            if (mainDataFrame.separationData & (1<<1))
                mainDataFrame.rocketState = FIRST_SEPAR;

            // Odcięcie bezpieczeństwa:
            else if (safetyCutoff_36atm && mainDataFrame.tankPressure < 36.0) {

                // Każ serwu zamknąć zawór:
                char messageOpen[] = "MNVL;0";
                if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
                    mainDataFrame.espNowErrorCounter++;
            }
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

            // Otworzenie zaworu upustowego:
            xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);

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