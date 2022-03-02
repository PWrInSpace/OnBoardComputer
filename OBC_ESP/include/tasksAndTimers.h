#ifndef TASKS_AND_TIMERS
#define TASKS_AND_TIMERS

#include "mainStructs.h"
#include "dataStructs.h"
#include "timers.h"
#include "now.h"

//Tasks
void loraTask(void *arg);
void rxHandlingTask(void *arg);

void stateTask(void *arg);
void dataTask(void *arg);
void sdTask(void *arg);
void flashTask(void *arg);

//Timers
void watchdogTimerCallback(TimerHandle_t xTimer);

void disconnectTimerCallback(TimerHandle_t xTimer);


#endif