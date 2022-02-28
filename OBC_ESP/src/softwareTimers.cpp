#include "timers.h"
#include "mainStructs.h"

extern WatchdogTimer wt;
extern RocketControl rc;

void watchdogTimerCallback(TimerHandle_t xTimer){
  Serial.println("WATCHDOG TIMER"); //DEBUG

  if(wt.LoRaTaskFlag == false){
    Serial.println("WATCHDOG RESET");
    wt.reset(rc.state);
  }

  /*
  if(wt.RXhandlingTaskFlag == false){
  }
  */

 wt.setFlags(false);
}


void disconnectTimerCallback(TimerHandle_t xTimer){
  rc.changeState(ABORT);
}