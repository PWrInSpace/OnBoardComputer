#include "../include/timers/watchdog.h"
#include "../include/structs/rocketControl.h"
#include "data_structs.h"
#include "../include/structs/stateMachine.h"

extern WatchdogTimer wt;
extern RocketControl rc;

void watchdogTimerCallback(TimerHandle_t xTimer){
 //Serial.println("WATCHDOG TIMER"); //DEBUG
  /*
  if(wt.loraTaskFlag == false){
    if(SM_getCurrentState() < COUNTDOWN || SM_getCurrentState() > FIRST_STAGE_RECOVERY){
      wt.reset(SM_getCurrentState());
    }else{
      //error handling
      //dataFrame.errors.setWatchDogError(WATCHDOG_LORA_ERROR);
      strcpy(log, "Watchdog timer LORA");
      rc.sendLog(log);
    }
  }

  if(wt.rxHandlingTaskFlag == false){
    if(SM_getCurrentState() < COUNTDOWN){
      wt.reset(SM_getCurrentState());
    }else if(SM_getCurrentState() == COUNTDOWN){ //esp now down [*]
      //error handling
      SM_changeStateRequest(States::ABORT);
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
    wt.reset(SM_getCurrentState());
  }

  if(wt.dataTaskFlag == false){
    wt.reset(SM_getCurrentState());
  }

  if(wt.sdTaskFlag == false && wt.flashTaskFlag == false){
    wt.reset(SM_getCurrentState());
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
    
  if(SM_changeStateRequest(States::ABORT) == false){
    rc.sendLog("Disconnect timer abort request rejected");
  }
  
  rc.sendLog("Disconnect timer");
}

void espNowConnectionCallback(TimerHandle_t xTimer){

  if(rc.isConnectedFlags[TANWA] == false){
    rc.sendLog("Tanwa not connected");
  }

  if(rc.isConnectedFlags[PITOT] == false){
    rc.dataFrame.pitot.wakenUp = 0;
    rc.sendLog("Pitot not connected");
  }
  
  if(rc.isConnectedFlags[MAIN_VALVE] == false){
    rc.dataFrame.mainValve.wakeUp = 0;
    rc.sendLog("Main valve not connected");
  }
  
  if(rc.isConnectedFlags[UPUST_VALVE] == false){
    rc.dataFrame.upustValve.wakeUp = 0;
    rc.sendLog("Upus not connected");
  }
  
  if(rc.isConnectedFlags[BLACK_BOX] == false){
    rc.dataFrame.blackBox.wakeUp = 0;
    rc.sendLog("BlackBox not connected");
  }
  
  if(rc.isConnectedFlags[PAYLOAD] == false){
    rc.dataFrame.payload.wakenUp = 0;
    rc.sendLog("Payload not connected");
  }

  Serial.println("---ESP NOW CALLBACK---");
  rc.connectedStatus = 0x00;
  for(int i = 0; i < CONNECTION_CHECK_DEVICE_NUMBER; ++i){
    Serial.println(rc.isConnectedFlags[i]);
    
    rc.connectedStatus |= ((uint8_t)rc.isConnectedFlags[i] << i);
  }
  Serial.println("---END ESP NOW CALLBACK---");
  
  memset(rc.isConnectedFlags, 0, CONNECTION_CHECK_DEVICE_NUMBER);
  
  // Serial.println("---SET---");
  
  // for(int i = 0; i < CONNECTION_CHECK_DEVICE_NUMBER; ++i){
  //   Serial.println(rc.isConnectedFlags[i]);
  // }
  // Serial.println("---END SET---");
  
}