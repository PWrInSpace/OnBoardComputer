#include "StateChanger.h"

extern MainDataFrame mainDataFrame;
StateChanger stateChanger;

// Jednorazowa akcja wykonywana przy przechodzeniu do Abortu:
void StateChanger::going2abort() {

    // Otworzenie upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);

    // Zwolnienie pitota:
    mainDataFrame.pitotPeriod = 20000;
}

/**********************************************************************************************/

void StateChanger::idle2fueling() {

    // Każ serwu zamknąć zawór:
    char messageOpen[] = "MNVL;0";
    if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
        mainDataFrame.espNowErrorCounter++;

    // Zamknij także upustowy:
    xTaskCreate(valveClose, "Task close valve", 4096, NULL, 2, NULL);

    // Ustawienie pitota co 5s:
    mainDataFrame.pitotPeriod = 5000;
}

/**********************************************************************************************/

void StateChanger::fueling2countdown() {

    // Ustawienie pitota co 1s:
    mainDataFrame.pitotPeriod = 1000;
}

/**********************************************************************************************/

void StateChanger::countdown2flight() {

    // Ustawienie pitota co 50ms:
    mainDataFrame.pitotPeriod = 50;
}

/**********************************************************************************************/

void StateChanger::flight2firstSepar() {

    // Spowolnij pomiary z pitota:
    uint16_t pitotPeriod = FLIGHT_DATA_PERIOD*10;
    if(esp_now_send(adressPitot, (uint8_t *) &pitotPeriod, sizeof(pitotPeriod)))
        mainDataFrame.espNowErrorCounter++;

    // Ustawienie pitota co 500ms:
    mainDataFrame.pitotPeriod = 500;

    // Każ serwu zamknąć zawór:
    char messageOpen[] = "MNVL;0";
    if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
        mainDataFrame.espNowErrorCounter++;
}

/**********************************************************************************************/

void StateChanger::firstSep2secSep() {

    // Otworzenie zaworu upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);

    // Ustawienie pitota co 1s:
    mainDataFrame.pitotPeriod = 1000;
}