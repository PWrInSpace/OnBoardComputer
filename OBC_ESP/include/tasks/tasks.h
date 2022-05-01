#ifndef TASKS_HH
#define TASKS_HH

#include <LITTLEFS.h>
#include <LoRa.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include "../include/structs/rocketControl.h"
#include "../include/structs/dataStructs.h"
#include "../include/timers/watchdog.h"
#include "../include/com/now.h"
#include "pinout.h"
#include "../include/components/SDCard.h"
#include "../include/structs/stateMachine.h"

extern RocketControl rc;
extern WatchdogTimer wt;
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

// Functions
void parseR4A(char* data);
void parseR4O(char* data);

#endif