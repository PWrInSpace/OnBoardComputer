#include "timers.h"
#include "mainStructs.h"
#include "dataStructs.h"

extern WatchdogTimer wt;
extern RocketControl rc;
extern DataFrame dataFrame;

void watchdogTimerCallback(TimerHandle_t xTimer){
 //Serial.println("WATCHDOG TIMER"); //DEBUG
  
  if(wt.loraTaskFlag == false){
    if(rc.state < COUNTDOWN || rc.state > FIRST_STAGE_RECOVERY){
      wt.reset(rc.state);
    }else{
      //error handling
      dataFrame.errors.watchdog = WATCHDOG_LORA_ERROR;
      rc.sendLog("Watchdog timer LoRa in state: " + String(rc.state));
    }
  }

  if(wt.rxHandlingTaskFlag == false){
    if(rc.state < COUNTDOWN){
      wt.reset(rc.state);
    }else if(rc.state == COUNTDOWN){ //esp now down [*]
      //error handling
      rc.changeState(StateMachine::ABORT);
      dataFrame.errors.watchdog = WATCHDOG_RX_HANDLING_ERROR;
      rc.sendLog("Watchdog timer rxHandling in ABORT state");
    }else{
      //error handling
      dataFrame.errors.watchdog = WATCHDOG_RX_HANDLING_ERROR;
      rc.sendLog("Watchdog timer rxHandling in state: " + String(rc.state));
    }
  }

  if(wt.stateTaskFlag == false){
    wt.reset(rc.state);
  }

  if(wt.dataTaskFlag == false){
    wt.reset(rc.state);
  }

  if(wt.sdTaskFlag == false && wt.flashTaskFlag == false){
    wt.reset(rc.state);
  }else if(wt.sdTaskFlag == false){
    //error handling
    dataFrame.errors.watchdog = WATCHDOG_SD_ERROR;
    rc.sendLog("Watchdog timer SD in state: " + String(rc.state));
  }else if(wt.flashTaskFlag == false){
    //error handling
    dataFrame.errors.watchdog = WATCHDOG_FLASH_ERROR;
    rc.sendLog("Watchdog timer flash in state: " + String(rc.state));
  }

  wt.setFlags(false);
}


void disconnectTimerCallback(TimerHandle_t xTimer){
  if(rc.state <= COUNTDOWN){
    rc.changeState(ABORT);
  }
  rc.sendLog("Disconnect Timer");
}