#include "tasksAndTimers.h"
#include "SDcard.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;
extern SPIClass mySPI;

//TODO //FIXME
// [ ] states in other tasks can be in new state before state setup end

void stateTask(void *arg){
  //StateMachineEvent currentEvent = IDLE_EVENT;  //prevent execute loop before setup
  TxDataEspNow txDataEspNow;

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
          if(dataFrame.upustValve.valveState == VALVE_CLOSE && dataFrame.mainValve.valveState == VALVE_CLOSE){
            rc.changeState(FUELING);
          }else{
            rc.options.upustValveRequestState = VALVE_CLOSE;
            rc.options.mainValveRequestState = VALVE_CLOSE;
            
            rc.unsuccessfulEvent();
            //valves are sleeping for 30 s, delay and waiting for request is not the best solution
          }
          break;

        case RDY_TO_LAUNCH_EVENT:
          rc.changeState(RDY_TO_LAUNCH);
          break;

        case COUNTDOWN_EVENT:
          //dataframe 
          if(dataFrame.allDevicesWokeUp() || rc.options.forceLaunch == true){
            //rc.options.upustValveRequestState = VALVE_CLOSE; //mayby good idea //IDK
            xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
            digitalWrite(CAMERA, HIGH); //turn on camera
            rc.options.dataFramePeriod = 100; //dataFrame create period

            //turn on mission timer
            rc.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(rc.missionTimer.isEnable()){
              rc.changeState(COUNTDOWN);
            }else{
              rc.unsuccessfulEvent();
            }
          }else{
            rc.unsuccessfulEvent();
          }

          break;

        case FLIGHT_EVENT:
          //open main valve request //TODO main valve send data time 100 ms
          txDataEspNow.setVal(100, VALVE_OPEN, 0); //IDK
          rc.options.mainValveRequestState = VALVE_OPEN;

          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          rc.changeState(FLIGHT);
          break;

        case FIRST_STAGE_RECOVERY_EVENT:
          //i2c force 1 stage recovery
          txDataEspNow.setVal(500, VALVE_CLOSE, 0);
          rc.options.mainValveRequestState = VALVE_CLOSE;

          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          rc.changeState(FIRST_STAGE_RECOVERY);
          break;

        case SECOND_STAGE_RECOVERY_EVENT:
          //i2c force 2 stage recovery
          txDataEspNow.setVal(500, VALVE_OPEN, 0);
          rc.options.upustValveRequestState = VALVE_OPEN;

          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          rc.changeState(SECOND_STAGE_RECOVERY);
          break;

        case ON_GROUND_EVENT:
          rc.options.dataFramePeriod = 10000;
          
          rc.changeState(ON_GROUND);
          break;

        case ABORT_EVENT:
          xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
      
          txDataEspNow.setVal(500, VALVE_OPEN, 0);
          rc.options.mainValveRequestState = VALVE_OPEN;

          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          rc.options.dataFramePeriod = 10000;
      
          rc.changeState(ABORT);
          break;

        default:
          //TODO log error
          ESP.restart();
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
  
      Serial.print("TASK: "); Serial.println(rc.state); //DEBUG
      Serial.print("EVENT: "); Serial.println(rc.stateEvent); //DEBUG
    }

    switch(rc.state){
      case COUNTDOWN:
        if(rc.missionTimer.getTime() < rc.options.ignitionTime && dataFrame.ignition == false){
          txDataEspNow.setVal(420, IGNITION_COMMAND, 0);

          //send ignition request
          esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          dataFrame.ignition = true;
        }

        if(rc.missionTimer.getTime() > 0){
          rc.changeStateEvent(FLIGHT_EVENT);
        }else{
          Serial.print("T: "); //DEBUG
          Serial.println(rc.missionTimer.getTime() / 1000); //DEBUG
        }

        break;

      default:
        break;
    }
    

    //Serial.println("State TASK"); //DEBUG

    wt.stateTaskFlag = true;
    vTaskDelay(1000 / portTICK_PERIOD_MS); //DEBUG TIME
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