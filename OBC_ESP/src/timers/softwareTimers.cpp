#include "../include/timers/watchdog.h"
#include "../include/structs/mainStructs.h"
#include "../include/structs/dataStructs.h"

extern WatchdogTimer wt;
extern RocketControl rc;
extern DataFrame dataFrame;

void watchdogTimerCallback(TimerHandle_t xTimer){
 //Serial.println("WATCHDOG TIMER"); //DEBUG
  char log[SD_FRAME_ARRAY_SIZE] = {};
  
  if(wt.loraTaskFlag == false){
    if(rc.state < COUNTDOWN || rc.state > FIRST_STAGE_RECOVERY){
      wt.reset(rc.state);
    }else{
      //error handling
      dataFrame.errors.setWatchDogError(WATCHDOG_LORA_ERROR);
      strcpy(log, "Watchdog timer LORA");
      rc.sendLog(log);
    }
  }

  if(wt.rxHandlingTaskFlag == false){
    if(rc.state < COUNTDOWN){
      wt.reset(rc.state);
    }else if(rc.state == COUNTDOWN){ //esp now down [*]
      //error handling
      rc.changeState(StateMachine::ABORT);
      dataFrame.errors.setWatchDogError(WATCHDOG_RX_HANDLING_ERROR);
      
      strcpy(log, "Watchdog timer rxHandling");
      rc.sendLog(log);
    }else{
      dataFrame.errors.setWatchDogError(WATCHDOG_RX_HANDLING_ERROR);
      
      strcpy(log, "Watchdog timer rxHandling");
      rc.sendLog(log);
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
    dataFrame.errors.setWatchDogError(WATCHDOG_SD_ERROR);
    
    strcpy(log, "Watchdog timer SD");
    rc.sendLog(log);
  }else if(wt.flashTaskFlag == false){
    dataFrame.errors.setWatchDogError(WATCHDOG_FLASH_ERROR);
    
    strcpy(log, "Watchdog timer flash");
    rc.sendLog(log);
  }

  wt.setFlags(false);
}


void disconnectTimerCallback(TimerHandle_t xTimer){
  if(rc.state <= COUNTDOWN){
    rc.changeState(ABORT);
  }
  
  char log[] = "Disconnect Timer";
  rc.sendLog(log);
}