#include "tasksAndTimers.h"
#include "SDcard.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;
extern SPIClass mySPI;

void stateTask(void *arg){
  TxDataEspNow txDataEspNow;

  while(1){
    if(ulTaskNotifyTake(pdTRUE, 0)){
      //portENTER_CRITICAL(&rc.stateLock);
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
            digitalWrite(CAMERA, HIGH); //turn on camera
            
            //set options
            rc.options.upustValveRequestState = VALVE_CLOSE; //mayby good idea //IDK at the moment is usless, becaus slaves has while loop block 
            rc.options.sdDataPeriod = rc.options.sdFastPeriod * portTICK_PERIOD_MS;
            rc.options.loraDataPeriod = rc.options.loraFastPeriod * portTICK_PERIOD_MS;

            //turn on mission timer
            dataFrame.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(dataFrame.missionTimer.isEnable()){
              if(xTimerDelete(rc.disconnectTimer, 25) == pdFALSE){
                rc.sendLog("Timer delete error");
              } //turn off disconnectTimer
              rc.disconnectTimer = NULL;
              
              rc.changeState(COUNTDOWN);
            }else{
              dataFrame.errors.setLastException(MISSION_TIMER_EXCEPTION);
              rc.unsuccessfulEvent();
            }
          }else{
            dataFrame.errors.setLastException(WAKE_UP_EXCEPTION);
            rc.unsuccessfulEvent();
          }

          break;

        case FLIGHT_EVENT:
          //open main valve request //TODO main valve send data time 100 ms
          txDataEspNow.setVal(100, VALVE_OPEN, 0); //IDK

          rc.options.mainValveRequestState = VALVE_OPEN;

          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          //set options
          rc.options.flashDataPeriod = rc.options.flashFastPeriod * portTICK_PERIOD_MS;

          rc.changeState(FLIGHT);
          break;

        case FIRST_STAGE_RECOVERY_EVENT:
          //i2c force 1 stage recovery
          txDataEspNow.setVal(500, VALVE_CLOSE, 0);
          rc.options.mainValveRequestState = VALVE_CLOSE;
          
          rc.options.flashDataPeriod = rc.options.flashSlowPeriod * portTICK_PERIOD_MS;

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
          rc.options.sdDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.loraDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;

          //dataFrame.missionTimer.turnOffTimer();
          
          rc.changeState(ON_GROUND);
          break;

        case ABORT_EVENT:
          xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
      
          txDataEspNow.setVal(500, VALVE_OPEN, 0);
          rc.options.mainValveRequestState = VALVE_OPEN;

          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          rc.options.sdDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.loraDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;

          rc.changeState(ABORT);
          break;

        default:
          rc.sendLog("Unknown state event, State_event: " +  String(rc.stateEvent));
          ESP.restart();
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    switch(rc.state){
      case COUNTDOWN:
        if(dataFrame.missionTimer.getTime() >= rc.options.ignitionTime && dataFrame.ignition == false){
          txDataEspNow.setVal(420, IGNITION_COMMAND, 0);  //IDK
          //send ignition request
          esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          dataFrame.ignition = true;
        }

        if(dataFrame.missionTimer.getTime() > 0){
          rc.changeStateEvent(FLIGHT_EVENT);
        }

        break;
      default:
        break;
    }
    

    //Serial.println("State TASK"); //DEBUG

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}


void dataTask(void *arg){
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  String sd;
  String lora;

  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("DATA: "); Serial.println(xTaskGetTickCount()); //DEBUG

      //read data from sensors and gps
      //i2c spi ect...
      
      //read i2c comm data
      //rc.sendLog("Hello space!");
      //filters

      //compute data
      /* //DEBUG disable
      if((dataFrame.upustValve.tankPressure < rc.options.tankMinPressure && dataFrame.mainValve.valveState == VALVE_OPEN) && rc.state > COUNTDOWN){
        //close valve
        rc.options.mainValveRequestState = VALVE_CLOSE;
      }*/

      if(rc.state == FLIGHT && dataFrame.recovery.firstStageDone == true){
        rc.changeStateEvent(FIRST_STAGE_RECOVERY_EVENT);
      }else if(rc.state == FIRST_STAGE_RECOVERY && dataFrame.recovery.secondStageDone == true){
        rc.changeState(SECOND_STAGE_RECOVERY);
      }
    }

    //LoRa
    if((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraDataPeriod){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      //Serial.print("LoRa data "); Serial.println(xTaskGetTickCount());//DEBUG
      lora = dataFrame.createLoRaFrame(rc.state, rc.getDisconnectRemainingTime());

      if(xQueueSend(rc.loraTxQueue, (void*)&lora, 0) != pdTRUE){
        dataFrame.errors.setRTOSError(RTOS_LORA_QUEUE_ADD_ERROR);
        rc.sendLog("LoRa queue full");
      }
      dataFrame.errors.reset(ERROR_RESET_LORA);
    }

    //FLASH
    if((rc.state > COUNTDOWN && rc.state < ON_GROUND) && rc.options.flashWrite){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
       // Serial.print("Flash save ");  Serial.println(xTaskGetTickCount()); //DEBUG
        if(xQueueSend(rc.flashQueue, (void*)&dataFrame, 0) != pdTRUE){
          dataFrame.errors.setRTOSError(RTOS_FLASH_QUEUE_ADD_ERROR);
          rc.sendLog("Flash queue full");
        }
      }
        
        //TODO send data to blackbox
    }

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataPeriod){
      //Serial.println(xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) ;
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("SD save "); Serial.println(xTaskGetTickCount()); //DEBUG
      sd = dataFrame.createSDFrame(rc.state, rc.getDisconnectRemainingTime(), rc.options);
      
      if(xQueueSend(rc.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        //Serial.println("Log"); //DEBUG
        dataFrame.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  

      dataFrame.errors.reset(ERROR_RESET_SD); //reset errors after save   //IDK
    }
  
    wt.dataTaskFlag = true;
    vTaskDelay(10/ portTICK_PERIOD_MS);  //OPTIMIZE IDK WHY time is a multiple of four
  }
}

void sdTask(void *arg){
  //SDCard sd(mySPI, SD_CS);
  String data;
  while(1){
    //Serial.println("sd TASK"); //DEBUG
    if(xQueueReceive(rc.sdQueue, (void*)&data, 10) == pdTRUE){
      
      if(data.startsWith("LOG")){
        Serial.println(data); //DEBUG
        if(xSemaphoreTake(rc.spiMutex, 10) == pdTRUE){
          //sdwrite
          xSemaphoreGive(rc.spiMutex);
        }else{
          dataFrame.errors.setRTOSError(RTOS_SPI_MUTEX_ERROR);
          Serial.println("MUTEX ERROR"); //DEBUG
        }
      }else{
        Serial.println(data); //DEBUG
        if(xSemaphoreTake(rc.spiMutex, 10) == pdTRUE){
          //sdwrite
          xSemaphoreGive(rc.spiMutex);
        }else{
          dataFrame.errors.setRTOSError(RTOS_SPI_MUTEX_ERROR);
          
          Serial.println("MUTEX ERROR"); //DEBUG
        }
      }
    }

    wt.sdTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void flashTask(void *arg){
  DataFrame frame;

  while(1){
    //Serial.println("flash TASK"); //DEBUG
    if(xQueueReceive(rc.flashQueue, (void*)&frame, 10) == pdTRUE){
        //sdwrite
    }

    wt.flashTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}