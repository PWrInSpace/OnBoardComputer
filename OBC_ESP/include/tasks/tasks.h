#ifndef TASKS_HH
#define TASKS_HH

#include <LITTLEFS.h>
#include <LoRa.h>
#include <SparkFun_u-blox_GNSS_Arduino_Library.h>

#include "../include/structs/rocketControl.h"
#include "data_structs.h"
#include "../include/timers/watchdog.h"
#include "../include/com/now.h"
#include "pinout.h"
#include "../include/components/SDcard.h"
#include "../include/structs/stateMachine.h"
#include "SparkFun_LPS25HB_Arduino_Library.h"
#include "../sensors/imuAPI.h"

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
void espNowConnectionCallback(TimerHandle_t xTimer);

// Functions
void parseR4A(char* data);
void parseR4O(char* data);

#endif