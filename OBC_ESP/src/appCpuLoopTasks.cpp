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
  //StateMachineEvent currentEvent = IDLE_EVENT;  //prevent execute loop before setup
 
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 25)){
      //portENTER_CRITICAL(&rc.stateLock);
      //currentState = rc.state;
      //Serial.print("State task setup: "); //DEBUG
      //Serial.println(currentState); //DEBUG
      //setup
      
      switch(rc.stateEvent){
        case IDLE_EVENT:
          rc.changeState(IDLE);
          break;
        case ARMED_EVENT:
          //recovery arm request
          //check recovery arm answer
          
          rc.changeState(ARMED);
          break;
        case FUELING_EVENT:
          
          rc.changeState(FUELING);
          break;
        case RDY_TO_LAUNCH_EVENT:
          rc.changeState(RDY_TO_LAUNCH);
          break;
        case COUNTDOWN_EVENT:
          //dataframe 
          //xTimerDelete(rc.disconnectTimer);
          rc.changeState(COUNTDOWN);
          break;
        case FLIGHT_EVENT:
          rc.changeState(FLIGHT);
          break;
        case FIRST_STAGE_RECOVERY_EVENT:
          rc.changeState(FIRST_STAGE_RECOVERY);
          break;
        case SECOND_STAGE_RECOVERY_EVENT:
          rc.changeState(SECOND_STAGE_RECOVERY);
          break;
        case ON_GROUND_EVENT:
        
          rc.changeState(ON_GROUND);
          break;
        case ABORT_EVENT:
          rc.changeState(ABORT);
          break;
        default:
          //TODO log error
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    switch(rc.state){
      case COUNTDOWN:
        break;
      default:
        break;
    }

    //Serial.println("State TASK"); //DEBUG

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