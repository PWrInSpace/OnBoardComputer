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
            rc.options.sdDataPeriod = 100; //dataFrame create period

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
          rc.options.sdDataPeriod = 10000;
          rc.options.dataFramePeriod = 10000;
          rc.options.loraDataPeriod = 5000;
          
          rc.changeState(ON_GROUND);
          break;

        case ABORT_EVENT:
          xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
      
          txDataEspNow.setVal(500, VALVE_OPEN, 0);
          rc.options.mainValveRequestState = VALVE_OPEN;

          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          rc.options.sdDataPeriod = 10000;
          rc.options.dataFramePeriod = 10000;
          rc.options.loraDataPeriod = 5000;

          rc.changeState(ABORT);
          break;

        default:
          rc.sendLog("Unknown state event, State_event: " +  String(rc.stateEvent));
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


void dataTask(void *arg){
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  String data;
  String log;

  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("DATA: "); Serial.println(xTaskGetTickCount()); //DEBUG

      //read data from sensors and gps
      uint32_t rand = esp_random() % 100; //DEBUG
      //Serial.println(rand); //DEBUG
      //data = String(rand);
      vTaskDelay(rand / portTICK_PERIOD_MS); //DEBUG

      //i2c spi ect...
      data = dataFrame.createSDFrame();
      
      //read i2c comm data
      //rc.sendLog("Hello space!");
      //filters

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
    }

    //LoRa
    if((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraDataPeriod){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      //Serial.print("LoRa data "); Serial.println(xTaskGetTickCount());//DEBUG
      if(xQueueSend(rc.loraTxQueue, (void*)&data, 0) != pdTRUE){
        dataFrame.errors.rtos = RTOS_QUEUE_ADD_ERROR;
        rc.sendLog("LoRa queue full");
      }
    }

    //FLASH
    if((rc.state > COUNTDOWN && rc.state < ON_GROUND) && rc.options.flashWrite){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        //Serial.print("Flash save ");  Serial.println(xTaskGetTickCount()); //DEBUG
        if(xQueueSend(rc.flashQueue, (void*)&dataFrame, 0) != pdTRUE){
          dataFrame.errors.rtos = RTOS_QUEUE_ADD_ERROR;
          rc.sendLog("Flash queue full");
        }
      }
        
        //TODO send data to blackbox
    }

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("SD save "); Serial.println(xTaskGetTickCount()); //DEBUG
      if(xQueueSend(rc.sdQueue, (void*)&data, 0) != pdTRUE){ //data to SD
        //Serial.println("Log"); //DEBUG
        dataFrame.errors.rtos = RTOS_QUEUE_ADD_ERROR;
      }  

      dataFrame.errors.reset(); //reset errors after save   //IDK
    }
  
    wt.dataTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);  //1 the highest accurate
  }
}

void sdTask(void *arg){
  //SDCard sd(mySPI, SD_CS);
  String data;
  while(1){
    //Serial.println("sd TASK"); //DEBUG
    if(xQueueReceive(rc.sdQueue, (void*)&data, 0) == pdTRUE){
      if(data.startsWith("LOG")){
        Serial.println(data);
        //SD write log
      }else{
        Serial.println(data);
        //SD write data
      }
    }

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

void flashTask(void *arg){

  while(1){
    //Serial.println("flash TASK"); //DEBUG

    wt.flashTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}