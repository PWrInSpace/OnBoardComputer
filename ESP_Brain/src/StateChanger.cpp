#include "StateChanger.h"

extern MainDataFrame mainDataFrame;

// Jednorazowa akcja wykonywana przy przechodzeniu do Abortu:
void StateChanger::going2abort() {

    // Otworzenie upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
}

/**********************************************************************************************/

void StateChanger::idle2fueling() {

    // Każ serwu zamknąć zawór:
    char messageOpen[] = "MNVL;0";
    if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
        mainDataFrame.espNowErrorCounter++;

    // Zamknij także upustowy:
    xTaskCreate(valveClose, "Task close valve", 4096, NULL, 2, NULL);
}

/**********************************************************************************************/

void StateChanger::fueling2countdown() {

}

/**********************************************************************************************/

void StateChanger::countdown2flight() {

}

/**********************************************************************************************/

void StateChanger::flight2firstSepar() {

    mainDataFrame.forceSeparation = true;
}

/**********************************************************************************************/

void StateChanger::firstSep2secSep() {

    mainDataFrame.forceSeparation = true;

    // Otworzenie zaworu upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
}