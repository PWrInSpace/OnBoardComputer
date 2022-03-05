#include "tasksAndTimers.h"
#include "SDcard.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;
extern SPIClass mySPI;

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
          //TODO in future, check valves state

          rc.options.upustValveRequestState = VALVE_CLOSE;
          rc.options.mainValveRequestState = VALVE_CLOSE;
            
          rc.changeState(FUELING);
          break;

        case RDY_TO_LAUNCH_EVENT:
          rc.changeState(RDY_TO_LAUNCH);
          break;

        case COUNTDOWN_EVENT:
          //dataframe 
          if(dataFrame.allDevicesWokeUp() || rc.options.forceLaunch == true){
            xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
            digitalWrite(CAMERA, HIGH); //turn on camera
            
            //set options
            rc.options.upustValveRequestState = VALVE_CLOSE; //mayby good idea //IDK at the moment is usless, becaus slaves has while loop block 
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
          
          //set options
          rc.options.flashDataPeriod = 100; 

          rc.changeState(FLIGHT);
          break;

        case FIRST_STAGE_RECOVERY_EVENT:
          //i2c force 1 stage recovery
          txDataEspNow.setVal(500, VALVE_CLOSE, 0);
          rc.options.mainValveRequestState = VALVE_CLOSE;
          rc.options.dataFramePeriod = 250;
          rc.options.flashDataPeriod = 500;

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
      Serial.print("EVENT TUTAJ: "); Serial.println(rc.stateEvent); //DEBUG
    }

    switch(rc.state){
      case COUNTDOWN:
        if(rc.missionTimer.getTime() > rc.options.ignitionTime && dataFrame.ignition == false){
          txDataEspNow.setVal(420, IGNITION_COMMAND, 0);  //IDK
          Serial.println("Ignition"); //DEBUG
          //send ignition request
          esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          dataFrame.ignition = true;
        }

        if(rc.missionTimer.getTime() > 0){
          rc.changeStateEvent(FLIGHT_EVENT);
        }else{ //DEBUG
          Serial.print("T: "); //DEBUG
          Serial.println(rc.missionTimer.getTime() / 1000); //DEBUG
        }

        break;

      default:
        break;
    }
    

    //Serial.println("State TASK"); //DEBUG

    wt.stateTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}


//TODO timers on millis() instead of xTaskGetTickCount()
//fix timer bugss, get data in const freq
void dataTask(void *arg){
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  String data;

  while(1){
    //Serial.println("data TASK"); //DEBUG
    //Serial.println((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer);
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      //Serial.println(rc.options.dataFramePeriod); //DEBUG
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;

      //read data from sensors and gps
      //i2c spi ect...
      
      //read i2c comm data

      //compute data
      if((dataFrame.upustValve.tankPressure < rc.options.tankMinPressure && dataFrame.mainValve.valveState == VALVE_OPEN) && rc.state > COUNTDOWN){
        //close valve
        rc.options.mainValveRequestState = VALVE_CLOSE;
      }

      if(rc.state == FLIGHT && dataFrame.recovery.firstStageDone == true){
        rc.changeStateEvent(FIRST_STAGE_RECOVERY_EVENT);
      }else if(rc.state == FIRST_STAGE_RECOVERY && dataFrame.recovery.secondStageDone == true){
        rc.changeState(SECOND_STAGE_RECOVERY);
      }
      
      if((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraDataPeriod){
        Serial.println("LoRa data");
        if(xQueueSend(rc.loraTxQueue, (void*)&data, 10) != pdTRUE){
          //TODO LOG error
        }
        loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      }


      if((rc.state > COUNTDOWN && rc.state < ON_GROUND) && rc.options.flashWrite){
        if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataPeriod){
          Serial.println("Flash save"); 
          if(xQueueSend(rc.flashQueue, (void*)&dataFrame, 10) != pdTRUE){
            //TODO LOG error
          }
          flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }
        
        //TODO send data to blackbox
      }

      Serial.println("SD save !");
      if(xQueueSend(rc.sdQueue, (void*)&data, 10) != pdTRUE){ //data to SD
        //TODO LOG error
      }       
      //processing
    }
    
    wt.dataTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
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