#ifndef SINGLETASKS_H
#define SINGLETASKS_H

#include "queue.h"
#include "now.h"
#include "Timer_ms.h"

String countStructData();

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
void valveOpen(void *arg);

#endif