#ifndef SINGLETASKS_H
#define SINGLETASKS_H

#include "queue.h"
#include "now.h"
#include "Timer_ms.h"
#include "StateChanger.h"

String countStructData();

void saveFrameHeaders();

void uart2Handler();

void sendData(String txData);

/*******************************************/

#define VALVE_PWM 19
#define VALVE1 5
#define VALVE2 18

const int valveFrequency = 30000;
const int valvePWMChanel = 0;
const int valveResolution = 8;

void valveInit();
void valveMove(const uint8_t & limitSwitchPIN, const uint8_t & highValvePIN, const uint8_t & valveSpeed = 255);
void valveOpen(void *arg);
void valveClose(void *arg);
void valveTimeOpen(void *arg);

#endif