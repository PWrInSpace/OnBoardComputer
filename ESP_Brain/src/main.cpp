#include <Arduino.h>

#include "LoopTasks.h"
#include "SingleTasks.h"
//#include "ota.h"

#define SERVO_DELAY_SECONDS 1

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

    delay(1500);
    mainDataFrame.rocketState = INIT;

    Serial.begin(115200);

    xTaskCreate(sdTask,                 "Task SD",      65536, NULL, 2, NULL);
    
    /*if(!nowInit())
        mainDataFrame.espNowErrorCounter = 2137; // Fatalny błąd.

    nowAddPeer(adressPitot, 0);
    nowAddPeer(adressMValve, 0);

    saveFrameHeaders();*/

    mainDataFrame.rocketState = IDLE;
    Serial2.begin(115200);
    Serial2.setTimeout(1);
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
    // TODO funkcja wysyłająca pomiary

    vTaskDelay(2 / portTICK_PERIOD_MS);
}