#include "../include/timers/watchdog.h"
#include "../include/structs/rocketControl.h"
#include "../include/structs/dataStructs.h"
#include "../include/structs/stateMachine.h"

extern WatchdogTimer wt;
extern RocketControl rc;

void watchdogTimerCallback(TimerHandle_t xTimer){
 //Serial.println("WATCHDOG TIMER"); //DEBUG
  /*
  if(wt.loraTaskFlag == false){
    if(StateMachine::getCurrentState() < COUNTDOWN || StateMachine::getCurrentState() > FIRST_STAGE_RECOVERY){
      wt.reset(StateMachine::getCurrentState());
    }else{
      //error handling
      //dataFrame.errors.setWatchDogError(WATCHDOG_LORA_ERROR);
      strcpy(log, "Watchdog timer LORA");
      rc.sendLog(log);
    }
  }

  if(wt.rxHandlingTaskFlag == false){
    if(StateMachine::getCurrentState() < COUNTDOWN){
      wt.reset(StateMachine::getCurrentState());
    }else if(StateMachine::getCurrentState() == COUNTDOWN){ //esp now down [*]
      //error handling
      StateMachine::changeStateRequest(States::ABORT);
      //dataFrame.errors.setWatchDogError(WATCHDOG_RX_HANDLING_ERROR);
      
      strcpy(log, "Watchdog timer rxHandling");
      rc.sendLog(log);
    }else{
      //dataFrame.errors.setWatchDogError(WATCHDOG_RX_HANDLING_ERROR);
      
      strcpy(log, "Watchdog timer rxHandling");
      rc.sendLog(log);
    }
  }

  if(wt.stateTaskFlag == false){
    wt.reset(StateMachine::getCurrentState());
  }

  if(wt.dataTaskFlag == false){
    wt.reset(StateMachine::getCurrentState());
  }

  if(wt.sdTaskFlag == false && wt.flashTaskFlag == false){
    wt.reset(StateMachine::getCurrentState());
  }else if(wt.sdTaskFlag == false){
    //dataFrame.errors.setWatchDogError(WATCHDOG_SD_ERROR);
    
    strcpy(log, "Watchdog timer SD");
    rc.sendLog(log);
  }else if(wt.flashTaskFlag == false){
    //dataFrame.errors.setWatchDogError(WATCHDOG_FLASH_ERROR);
    
    strcpy(log, "Watchdog timer flash");
    rc.sendLog(log);
  }
  */
  wt.setFlags(false);
}


void disconnectTimerCallback(TimerHandle_t xTimer){
    
  if(StateMachine::changeStateRequest(States::ABORT) == false){
    rc.sendLog("Disconnect timer abort request rejected");
  }
  
  rc.sendLog("Disconnect timer");
}