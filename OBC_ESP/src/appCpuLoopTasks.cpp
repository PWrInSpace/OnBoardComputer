#include "tasksAndTimers.h"
#include "SDcard.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;

void stateTask(void *arg){
  TxDataEspNow txDataEspNow;
  char log[LORA_FRAME_ARRAY_SIZE] = {};
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

          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
            
          rc.changeState(FUELING);
          break;

        case RDY_TO_LAUNCH_EVENT:
          digitalWrite(CAMERA, HIGH); //turn on camera
          rc.changeState(RDY_TO_LAUNCH);
          break;

        case COUNTDOWN_EVENT:
          //dataframe 
          if(dataFrame.allDevicesWokeUp() || rc.options.forceLaunch == true){
            
            //set options
            rc.options.sdDataPeriod = rc.options.sdFastPeriod * portTICK_PERIOD_MS;
            rc.options.loraDataPeriod = rc.options.loraFastPeriod * portTICK_PERIOD_MS;

            //turn on mission timer
            dataFrame.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(dataFrame.missionTimer.isEnable()){
              if(xTimerDelete(rc.disconnectTimer, 25) == pdFALSE){
                strcpy(log, "Timer delete error");
                rc.sendLog(log);
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
          txDataEspNow.setVal(VALVE_OPEN, 0); //IDK
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          //set options
          rc.options.flashDataPeriod = rc.options.flashFastPeriod * portTICK_PERIOD_MS;

          rc.changeState(FLIGHT);
          break;

        case FIRST_STAGE_RECOVERY_EVENT:
          //i2c force 1 stage recovery
          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));

          rc.options.flashDataPeriod = rc.options.flashSlowPeriod * portTICK_PERIOD_MS;
          
          rc.changeState(FIRST_STAGE_RECOVERY);
          break;

        case SECOND_STAGE_RECOVERY_EVENT:
          //i2c force 2 stage recovery
          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
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
      
          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          rc.options.sdDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;
          rc.options.loraDataPeriod = rc.options.sharedPeriod * portTICK_PERIOD_MS;

          rc.changeState(ABORT);
          break;

        default:
          strcpy(log, "Unknown state event");
          rc.sendLog(log);
          ESP.restart();
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    switch(rc.state){
      case COUNTDOWN:
        if(dataFrame.missionTimer.getTime() >= rc.options.ignitionTime && dataFrame.ignition == false){
          txDataEspNow.setVal(IGNITION_COMMAND, 0);  //IDK
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

/**********************************************************************************************/

void dataTask(void *arg){
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  char sd[SD_FRAME_ARRAY_SIZE] = {};
  char lora[LORA_FRAME_ARRAY_SIZE] = {};
  char log[SD_FRAME_ARRAY_SIZE] = {};

  SFE_UBLOX_GPS gps;
  gps.begin(rc.i2c1, 0x42);

  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("DATA: "); Serial.println(xTaskGetTickCount()); //DEBUG

      //read data from sensors and gps
      //i2c spi ect...

      // GPS:
      dataFrame.GPSalt  = gps.getAltitudeMSL();
      dataFrame.GPSlal  = gps.getLatitude();
      dataFrame.GPSlong = gps.getLongitude();
      dataFrame.GPSsat  = gps.getFixType();
      dataFrame.GPSsec  = gps.getSecond();

      // IMU:

      // TODO!!!

      // Recovery:
      Wire.requestFrom(3, sizeof(RecoveryData));
      if (!Wire.readBytes((uint8_t*) &dataFrame.recovery, sizeof(RecoveryData))) {
        // ERROR I2C
      }

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
      dataFrame.createLoRaFrame(rc.state, rc.getDisconnectRemainingTime(), lora);

      if(xQueueSend(rc.loraTxQueue, (void*)&lora, 0) != pdTRUE){
        dataFrame.errors.setRTOSError(RTOS_LORA_QUEUE_ADD_ERROR);
        strcpy(log, "LoRa queue full"); // log
        rc.sendLog(log);
      }
      dataFrame.errors.reset(ERROR_RESET_LORA);
    }

    //FLASH
    if((rc.state > COUNTDOWN && rc.state < ON_GROUND) && rc.options.flashWrite){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
       
        if(xQueueSend(rc.flashQueue, (void*)&dataFrame, 0) != pdTRUE){
          dataFrame.errors.setRTOSError(RTOS_FLASH_QUEUE_ADD_ERROR);
          strcpy(log, "Flash queue full");
          rc.sendLog(log);
        }
      }
        
        //TODO send data to blackbox
    }

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      dataFrame.createSDFrame(rc.state, rc.getDisconnectRemainingTime(), rc.options, sd);
    
      if(xQueueSend(rc.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        dataFrame.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  

      dataFrame.errors.reset(ERROR_RESET_SD); //reset errors after save  
    }

    wt.dataTaskFlag = true;
    vTaskDelay(10/ portTICK_PERIOD_MS);  
  }
}

/**********************************************************************************************/

void sdTask(void *arg){
  SDCard mySD(rc.mySPI, SD_CS);
  char data[SD_FRAME_ARRAY_SIZE] = {};
  String dataPath = dataFileName;
  String logPath = logFileName;
  uint8_t sd_i = 0;

  vTaskDelay(50 / portTICK_RATE_MS);

  xSemaphoreTake(rc.spiMutex, pdTRUE);

  while(!mySD.init()){
    dataFrame.errors.setSDError(SD_INIT_ERROR);
    Serial.println("SD INIT ERROR!");
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  while(mySD.fileExists(dataPath + String(sd_i) + ".txt")){
    sd_i++;
  }
  dataPath = dataPath + String(sd_i) + ".txt";
  logPath = logPath + String(sd_i) + ".txt";

  xSemaphoreGive(rc.spiMutex);

  while(1){
    
    if(xQueueReceive(rc.sdQueue, (void*)&data, 0) == pdTRUE){
      
      xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
      
        if(strncmp(data, "LOG", 3) == 0){
          if(!mySD.write(logPath, data)){
            dataFrame.errors.setSDError(SD_WRITE_ERROR);
          }
        }else{
          if(!mySD.write(dataPath, data)){
            dataFrame.errors.setSDError(SD_WRITE_ERROR);
          }
        }  

      xSemaphoreGive(rc.spiMutex);
    } 

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

/**********************************************************************************************/

void flashTask(void *arg){

  File file;
  DataFrame frame;
  LITTLEFS.begin(true);
  bool wipeFile = true;

  while(1){

    if (uxQueueMessagesWaiting(rc.flashQueue) > FLASH_QUEUE_LENGTH / 2) {

      // Wiping file for the first time to remove data from previous flights:
      if (wipeFile) {
        wipeFile = false;
        file = LITTLEFS.open("/file.txt", "w");
      }
      else file = LITTLEFS.open("/file.txt", "a");

      while (uxQueueMessagesWaiting(rc.flashQueue) > 0) {

        xQueueReceive(rc.flashQueue, &frame, portMAX_DELAY);
        file.write((uint8_t*) &frame, sizeof(frame));
      }

      file.close();
    }

    wt.flashTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}