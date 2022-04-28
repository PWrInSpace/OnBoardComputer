#include "../include/tasks/tasks.h"

void dataTask(void *arg){
  DataFrame dataFrame;
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  char sd[SD_FRAME_ARRAY_SIZE] = {};
  char lora[LORA_FRAME_ARRAY_SIZE] = {};
  char log[SD_FRAME_ARRAY_SIZE] = {};

  SFE_UBLOX_GNSS gps;

  if(gps.begin(rc.hardware.i2c2, 0x42, 10, false) == false){
    //TODO error handling
    Serial.println("GPS INIT ERROR!"); //DEBUG
  }

  rc.dataFrame.mcb.watchdogResets = wt.resetCounter;

  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataFramePeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //Serial.print("DATA Start: "); Serial.println(xTaskGetTickCount()); //DEBUG
      
      //read data from sensors and gps
      //i2c spi ect...
    
      rc.dataFrame.mcb.state = StateMachine::getCurrentState();
      // GPS:
      
      rc.dataFrame.mcb.GPSlal = gps.getLatitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSlong = gps.getLongitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSalt = gps.getAltitude(10) / 10.0E2;
      
      rc.dataFrame.mcb.GPSsat = gps.getSIV(10);
      rc.dataFrame.mcb.GPSsec = gps.getTimeValid(10);

      // IMU:

      // TODO!!!

      // Recovery:
      xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
      rc.recoveryStm.getRecoveryData((uint8_t*) &rc.dataFrame.recovery);
      xSemaphoreGive(rc.hardware.i2c1Mutex);

      //read i2c comm data
      //rc.sendLog("Hello space!");
      //filters

      //compute data
      /* //DEBUG disable
      if((dataFrame.upustValve.tankPressure < rc.options.tankMinPressure && dataFrame.mainValve.valveState == VALVE_OPEN) && StateMachine::getCurrentState() > COUNTDOWN){
        //close valve
        rc.options.mainValveRequestState = VALVE_CLOSE;
      }*/
      /*
      if(StateMachine::getCurrentState() == FLIGHT && dataFrame.recovery.firstStageDone == true){
        rc.changeStateEvent(FIRST_STAGE_RECOVERY_EVENT);
      }else if(StateMachine::getCurrentState() == FIRST_STAGE_RECOVERY && dataFrame.recovery.secondStageDone == true){
        rc.changeState(SECOND_STAGE_RECOVERY);
      }*/
      //Serial.print("DATA Stop: "); Serial.println(xTaskGetTickCount());
    }

    //LoRa
    if((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraPeriod){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      rc.createLoRaFrame(lora);
      //Serial.print("LORA: "); Serial.println(xTaskGetTickCount());
      Serial.print(lora);

      if(xQueueSend(rc.hardware.loraTxQueue, (void*)&lora, 0) != pdTRUE){
        rc.errors.setRTOSError(RTOS_LORA_QUEUE_ADD_ERROR);
        strcpy(log, "LoRa queue full"); // log
        rc.sendLog(log);
      }
      rc.errors.reset(ERROR_RESET_LORA);
    }

    //FLASH
    if((StateMachine::getCurrentState() > COUNTDOWN && StateMachine::getCurrentState() < ON_GROUND) && rc.options.flashWrite){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataCurrentPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
       
        if(xQueueSend(rc.hardware.flashQueue, (void*)&rc.dataFrame, 0) != pdTRUE){
          rc.errors.setRTOSError(RTOS_FLASH_QUEUE_ADD_ERROR);
          strcpy(log, "Flash queue full");
          rc.sendLog(log);
        }
      }
        
        //TODO send data to blackbox
    }

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataCurrentPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      rc.createSDFrame(sd);
      //Serial.print(sd);
      if(xQueueSend(rc.hardware.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        rc.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  
      
      rc.errors.reset(ERROR_RESET_SD); //reset errors after save  
    }

    wt.dataTaskFlag = true;
    vTaskDelay(10/ portTICK_PERIOD_MS);  
  }
}