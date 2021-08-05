#ifndef NOW_H
#define NOW_H

#include "esp_now.h"
#include "WiFi.h"
#include "dataStructs.h"

extern PitotData pitotdata;
extern uint16_t pitotDelayTime;

// Adresy:
const uint8_t adressMain[] = {0x94, 0x3C, 0xC6, 0x24, 0xE3, 0x54};
const uint8_t adressPitot[] = {0x94, 0xb9, 0x7e, 0xc2, 0xe6, 0x28}; // stary adres
const uint8_t adressMValve[] = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D};

// Init:
bool nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t* address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

#endif