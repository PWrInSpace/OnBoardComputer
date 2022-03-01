#include "timers.h"
#include "mainStructs.h"

extern WatchdogTimer wt;
extern RocketControl rc;

void watchdogTimerCallback(TimerHandle_t xTimer){
  Serial.println("WATCHDOG TIMER"); //DEBUG
  
  // DISABLE FOR CREATING AND TESTING TASKS
  /*
  if(wt.loraTaskFlag == false){
    if(rc.state < COUNTDOWN || rc.state > FIRST_STAGE_RECOVERY){
      wt.reset(rc.state);
    }else{
      //watchdog log //TODO
    }
  }

  if(wt.rxHandlingTaskFlag == false){
    if(rc.state < COUNTDOWN){
      wt.reset(rc.state);
    }else if(rc.state == COUNTDOWN){ //esp now down [*]
      rc.changeState(StateMachine::ABORT);
      //watchdog log //TODO
    }else{
      //watchdog log //TODO
    }
  }

  if(wt.stateTaskFlag == false){
    wt.reset(rc.state);
  }

  if(wt.sdTaskFlag == false && wt.flashTaskFlag == false){
    wt.reset(rc.state);
  }else if(wt.sdTaskFlag ^ wt.flashTaskFlag){
    //watchdog log //TODO
  }
  */
  wt.setFlags(false);
}


void disconnectTimerCallback(TimerHandle_t xTimer){
  rc.changeState(ABORT);
}