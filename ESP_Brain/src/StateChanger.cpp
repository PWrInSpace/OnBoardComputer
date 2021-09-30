#include "StateChanger.h"

extern MainDataFrame mainDataFrame;

// Jednorazowa akcja wykonywana przy przechodzeniu do Abortu:
void going2abort() {

    // Otworzenie upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
}

/**********************************************************************************************/

void idle2fueling() {

    // Każ serwu zamknąć zawór:
    char messageOpen[] = "MNVL;0";
    if(esp_now_send(adressMValve, (uint8_t *) messageOpen, strlen(messageOpen)))
        mainDataFrame.espNowErrorCounter++;

    // Zamknij także upustowy:
    xTaskCreate(valveClose, "Task close valve", 4096, NULL, 2, NULL);
}

/**********************************************************************************************/

void fueling2countdown() {


}

/**********************************************************************************************/

void countdown2flight() {

}

/**********************************************************************************************/

void flight2firstSepar() {

}

/**********************************************************************************************/

void firstSep2secSep() {

    // Otworzenie zaworu upustowego:
    xTaskCreate(valveOpen, "Task open valve", 4096, NULL, 2, NULL);
}