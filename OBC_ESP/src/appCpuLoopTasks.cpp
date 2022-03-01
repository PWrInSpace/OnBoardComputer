#include "mainStructs.h"
#include "timers.h"
#include "tasksAndTimers.h"
#include "SDcard.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern SPIClass mySPI;

//TODO //FIXME
// [ ] states in other tasks can be in new state before state setup end

void stateTask(void *arg){
  StateMachine currentState = IDLE;  //prevent execute loop before setup
 
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 25)){
      //portENTER_CRITICAL(&rc.stateLock);
      currentState = rc.state;
      Serial.print("State task setup: "); //DEBUG
      Serial.println(currentState); //DEBUG
      //setup
      
      switch(currentState){
        case INIT:
          break;
        case IDLE:
          break;
        case ARMED:
          //recovery arm request
          //check recovery arm answer
          break;
        case FUELING:
          break;
        case RDY_TO_LAUNCH:
          break;
        case COUNTDOWN:
          //dataframe 
          //xTimerDelete(rc.disconnectTimer);
          break;
        case FLIGHT:
          break;
        case FIRST_STAGE_RECOVERY:
          break;
        case SECOND_STAGE_RECOVERY:
          break;
        case ON_GROUND:
          break;
        case ABORT:
          break;
        default:
          //TODO log error
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
      if(currentState != rc.state){ //setup done and state changed to previous
        rc.changeState(currentState);
      }
    }

    //Serial.println("State TASK"); //DEBUG
    //loop
    switch(currentState){
      case COUNTDOWN:
        break;
      default:
        break;
    }

    wt.stateTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}



void dataTask(void *arg){

  while(1){
    //Serial.println("data TASK"); //DEBUG

    wt.dataTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void sdTask(void *arg){
  //SDCard sd(mySPI, SD_CS);
  while(1){
    //Serial.println("sd TASK"); //DEBUG
    //while(xQueueReceive(rc.sdQueue, (void*)&data));

    wt.sdTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

void flashTask(void *arg){

  while(1){
    //Serial.println("flash TASK"); //DEBUG

    wt.flashTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}