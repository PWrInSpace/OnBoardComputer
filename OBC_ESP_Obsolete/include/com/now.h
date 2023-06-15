#ifndef NOW_H
#define NOW_H

#include "esp_now.h"
#include "WiFi.h"
#include "data_structs.h"
#include "../include/structs/rocketControl.h"

// Init:
bool nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t* address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

#endif