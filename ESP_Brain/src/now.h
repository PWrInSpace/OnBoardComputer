#ifndef NOW_H
#define NOW_H

#include "esp_now.h"
#include "WiFi.h"
#include "dataStruct.h"

extern PitotData pitotdata;
extern uint16_t pitotDelayTime;

// Adresy:
const uint8_t adressMain[] = {0x84, 0xCC, 0xA8, 0x2D, 0xDA, 0x18};
const uint8_t adressPitot[] = {0x94, 0xb9, 0x7e, 0xc2, 0xe6, 0x28};

// Init:
bool nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t* address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len);

#endif