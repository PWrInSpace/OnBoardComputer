#ifndef NOW_H
#define NOW_H

#include "esp_now.h"
#include "WiFi.h"
#include "dataStructs.h"
#include "mainStructs.h"

// Adresy:
const uint8_t adressOBC[]       = {0xB4, 0xE6, 0x2D, 0x85, 0x49, 0x4D}; // TODO poprawny adres!!!
const uint8_t adressPitot[]     = {0x94, 0xB9, 0x7E, 0xC2, 0xE6, 0x28}; // TODO poprawny adres!!!
const uint8_t adressMValve[]    = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressUpust[]     = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressBlackBox[]  = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!
const uint8_t adressTanWa[]     = {0xE8, 0xDB, 0x84, 0xA5, 0x93, 0x5D}; // TODO poprawny adres!!!

// Init:
bool nowInit();

// Dodanie peera:
bool nowAddPeer(const uint8_t* address, uint8_t channel);

// Przerwania:
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len);

#endif