#include "../include/tasks/tasks.h"

void dataTask(void *arg){
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  char sd[SD_FRAME_ARRAY_SIZE] = {};
  char lora[LORA_FRAME_ARRAY_SIZE] = {};
  char log[SD_FRAME_ARRAY_SIZE] = {};

  SFE_UBLOX_GNSS gps;

  if(gps.begin(rc.i2c2, 0x42, 10, false) == false){
    //TODO error handling
    Serial.println("GPS INIT ERROR!"); //DEBUG
  }

  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("DATA Start: "); Serial.println(xTaskGetTickCount()); //DEBUG
      
      //read data from sensors and gps
      //i2c spi ect...
    
      dataFrame.state = rc.state;
      // GPS:
      
      dataFrame.GPSlal = gps.getLatitude(10) / 10.0E6;
      dataFrame.GPSlong = gps.getLongitude(10) / 10.0E6;
      dataFrame.GPSalt = gps.getAltitude(10) / 10.0E2;
      
      dataFrame.GPSsat = gps.getSIV(10);
      dataFrame.GPSsec = gps.getTimeValid(10);

      // IMU:

      // TODO!!!

      // Recovery:
      xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
      rc.recoveryStm.getRecoveryData((uint8_t*) &dataFrame.recovery);
      xSemaphoreGive(rc.i2c1Mutex);

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
      //Serial.print("DATA Stop: "); Serial.println(xTaskGetTickCount());
    }

    //LoRa
    if((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraPeriod){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      dataFrame.createLoRaDataFrame(rc.state, rc.getDisconnectRemainingTime(), lora);
      //Serial.print("LORA: "); Serial.println(xTaskGetTickCount());
      Serial.print(lora);

      if(xQueueSend(rc.loraTxQueue, (void*)&lora, 0) != pdTRUE){
        dataFrame.errors.setRTOSError(RTOS_LORA_QUEUE_ADD_ERROR);
        strcpy(log, "LoRa queue full"); // log
        rc.sendLog(log);
      }
      dataFrame.errors.reset(ERROR_RESET_LORA);
    }

    //FLASH
    if((rc.state > COUNTDOWN && rc.state < ON_GROUND) && rc.options.flashWrite){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataCurrentPeriod){
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
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataCurrentPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      dataFrame.createSDFrame(rc.state, rc.getDisconnectRemainingTime(), rc.options, sd);
      //Serial.print(sd);
      if(xQueueSend(rc.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        dataFrame.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  
      
      dataFrame.errors.reset(ERROR_RESET_SD); //reset errors after save  
    }

    wt.dataTaskFlag = true;
    vTaskDelay(10/ portTICK_PERIOD_MS);  
  }
}