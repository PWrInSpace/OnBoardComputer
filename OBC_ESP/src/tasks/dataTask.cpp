#include "../include/tasks/tasks.h"
#include "Adafruit_MCP9808.h"

void dataTask(void *arg){
  DataFrame dataFrame;
  ImuData imuData;
  LPS25HB pressureSensor;
  Adafruit_MCP9808 tempsensor = Adafruit_MCP9808(); 
  SFE_UBLOX_GNSS gps;
  ImuAPI imu(&rc.hardware.i2c2);

  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  char sd[SD_FRAME_ARRAY_SIZE] = {};
  char lora[LORA_FRAME_ARRAY_SIZE] = {};


  pressureSensor.begin(rc.hardware.i2c2, PRESSURE_SENSOR_ADRESS);

  if (pressureSensor.isConnected() == false){
    rc.sendLog("PRESSURE SENSOR ERROR");
    rc.errors.setSensorError(PRESSURE_SENSOR_INIT_ERROR);
  }

  if (tempsensor.begin(0x18) == false) {
    rc.sendLog("TEMP SENSOR INIT ERROR");
    rc.errors.setSensorError(TEMP_SENSOR_INIT_ERROR);
  }else{
    tempsensor.setResolution(1);
    tempsensor.wake();
  }

  if(!imu.begin()){
    rc.sendLog("IMU INIT ERROR");
    rc.errors.setSensorError(IMU_INIT_ERROR);
  }else{
    imu.setReg(A_16g, G_2000dps, B_200Hz, M_4g);
    imu.setInitPressure();
  }

  if(gps.begin(rc.hardware.i2c2, GPS_ADRESS, 10, false) == false){
    rc.sendLog("GPS INIT ERROR");
    rc.errors.setSensorError(GPS_INIT_ERROR);
  }
  rc.dataFrame.mcb.watchdogResets = wt.resetCounter;



  while(1){

    //data
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataCurrentPeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
      rc.dataFrame.mcb.state = StateMachine::getCurrentState();

      rc.dataFrame.mcb.batteryVoltage = 2.93/3635.0 * analogRead(BATTERY) * 43.0/10.0;
      
      // GPS:
      rc.dataFrame.mcb.GPSlal = gps.getLatitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSlong = gps.getLongitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSalt = gps.getAltitude(10) / 10.0E2;
      
      rc.dataFrame.mcb.GPSsat = gps.getSIV(10);
      rc.dataFrame.mcb.GPSsec = gps.getTimeValid(10);

      //LP26HB - pressure
      rc.dataFrame.mcb.pressure = pressureSensor.getPressure_hPa();
      rc.dataFrame.mcb.temp_lp25 = pressureSensor.getTemperature_degC();
      
      //MCP temp
      rc.dataFrame.mcb.temp_mcp = tempsensor.readTempC();

      // IMU:
      imu.readData();
      imuData = imu.getData();
      rc.dataFrame.mcb.imuData[0] = imuData.ax;
      rc.dataFrame.mcb.imuData[0] = imuData.ay;
      rc.dataFrame.mcb.imuData[0] = imuData.az;
      rc.dataFrame.mcb.imuData[0] = imuData.gx;
      rc.dataFrame.mcb.imuData[0] = imuData.gy;
      rc.dataFrame.mcb.imuData[0] = imuData.gz;
      rc.dataFrame.mcb.imuData[0] = imuData.mx;
      rc.dataFrame.mcb.imuData[0] = imuData.my;
      rc.dataFrame.mcb.imuData[0] = imuData.mz;
      rc.dataFrame.mcb.imuData[0] = imuData.temperature;
      rc.dataFrame.mcb.imuData[0] = imuData.pressure;
      rc.dataFrame.mcb.imuData[0] = imuData.altitude;

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
      }
      //Serial.print("DATA Stop: "); Serial.println(xTaskGetTickCount());
      */
    }


    //LoRa
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraCurrentPeriod) || ulTaskNotifyTake(pdTRUE, 0)){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      
      rc.dataFrame.mcb.state = StateMachine::getCurrentState(); //get the newest information about state
      rc.createLoRaFrame(lora);
      //Serial.print("Dlugos: ");
      //Serial.println(strlen(lora));
      Serial.print(lora);
      

      if(xQueueSend(rc.hardware.loraTxQueue, (void*)&lora, 0) != pdTRUE){
        rc.errors.setRTOSError(RTOS_LORA_QUEUE_ADD_ERROR);
        rc.sendLog("LoRa quque is full");
      }
      rc.errors.reset(ERROR_RESET_LORA);
    }

    //FLASH
    if((StateMachine::getCurrentState() > COUNTDOWN && StateMachine::getCurrentState() < ON_GROUND)){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataCurrentPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        rc.dataFrame.mcb.state = StateMachine::getCurrentState(); //get the newest information about state

        if(rc.options.flashWrite){
          if(xQueueSend(rc.hardware.flashQueue, (void*)&rc.dataFrame, 0) != pdTRUE){
            rc.errors.setRTOSError(RTOS_FLASH_QUEUE_ADD_ERROR);
            rc.sendLog("Flash queue is full");
          }
        }

        if(esp_now_send(adressBlackBox, (uint8_t*) &dataFrame, sizeof(dataFrame)) != ESP_OK){
          rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          rc.sendLog("Black box send error");
        }
      }
        
      
    }

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataCurrentPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      
      rc.dataFrame.mcb.state = StateMachine::getCurrentState(); //get the newest information about state
      rc.createSDFrame(sd);
      

      if(xQueueSend(rc.hardware.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        rc.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  
      
      rc.errors.reset(ERROR_RESET_SD); //reset errors after save  
    }

    wt.dataTaskFlag = true;
    vTaskDelay(10/ portTICK_PERIOD_MS);  
  }
}