#include "../include/tasks/tasks.h"
#include "Adafruit_MCP9808.h"



void dataTask(void *arg){
  SFE_UBLOX_GNSS gps;
  ImuAPI imu(&rc.hardware.i2c2);
  LPS25HB pressureSensor;
  Adafruit_MCP9808 tempsensor = Adafruit_MCP9808(); 

  //data handl,ing variables
  ImuData imuData;
  TickType_t dataUpdateTimer = 0;
  TickType_t loraTimer = 0;
  TickType_t flashTimer = 0;
  TickType_t sdTimer = 0;
  char sd[SD_FRAME_ARRAY_SIZE] = {};
  char lora[LORA_FRAME_ARRAY_SIZE] = {};

  //variables for calculations
  float launchPadAltitude = 0;
  float lastMaxAltitude = 0;
  TickType_t apogeeConfirmationTimer = 0;
  

  if (gps.begin(rc.hardware.i2c2, 0x42, 10) == false) //Connect to the u-blox module using Wire port
  {
    rc.sendLog("GPS INIT ERROR");
    rc.errors.setSensorError(GPS_INIT_ERROR);
  }

  gps.setI2COutput(COM_TYPE_UBX);
  
  if(!imu.begin()){
    rc.sendLog("IMU INIT ERROR");
    rc.errors.setSensorError(IMU_INIT_ERROR);
  }else{
    imu.setReg(A_16g, G_2000dps, B_200Hz, M_4g);
    vTaskDelay(250 / portTICK_PERIOD_MS);
    imu.setInitPressure();
    launchPadAltitude = imu.getAltitude();
  }
  
  
  pressureSensor.begin(rc.hardware.i2c2, PRESSURE_SENSOR_ADRESS);

  if (pressureSensor.isConnected() == false){
    rc.sendLog("PRESSURE SENSOR ERROR");
    rc.errors.setSensorError(PRESSURE_SENSOR_INIT_ERROR);
  }

  if (tempsensor.begin(0x18, &rc.hardware.i2c2) == false) {
    rc.sendLog("TEMP SENSOR INIT ERROR");
    rc.errors.setSensorError(TEMP_SENSOR_INIT_ERROR);
  }else{
    tempsensor.setResolution(1);
    tempsensor.wake();
  }
  

  while(1){
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS) - dataUpdateTimer) >= rc.options.dataCurrentPeriod){
      dataUpdateTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      
      rc.dataFrame.mcb.state = SM_getCurrentState();

      rc.dataFrame.mcb.batteryVoltage = 2.93/3635.0 * analogRead(BATTERY) * 43.0/10.0 + 0.5;
      //portENTER_CRITICAL(&rc.hardware.stateLock);
      rc.dataFrame.mcb.GPSlal = gps.getLatitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSlong = gps.getLongitude(10) / 10.0E6;
      rc.dataFrame.mcb.GPSalt = gps.getAltitude(10) / 10.0E2;
        
      rc.dataFrame.mcb.GPSsat = gps.getSIV(10);
      rc.dataFrame.mcb.GPSsec = gps.getTimeValid(10);
      //portEXIT_CRITICAL(&rc.hardware.stateLock);
      Serial.println("====GPS DATA====");
      Serial.print("Lat: ");
      Serial.println(rc.dataFrame.mcb.GPSlal);
      
      Serial.print("Long: ");
      Serial.println(rc.dataFrame.mcb.GPSlong);
      
      Serial.print("Alt: ");
      Serial.println(rc.dataFrame.mcb.GPSalt);
      
      Serial.print("Sat: ");
      Serial.println(rc.dataFrame.mcb.GPSsat);
      
      Serial.print("Valid: ");
      Serial.println(rc.dataFrame.mcb.GPSsec);

      // IMU:
      
      imu.readData();
      imuData = imu.getData();
      rc.dataFrame.mcb.imuData[0] = imuData.ax;
      rc.dataFrame.mcb.imuData[1] = imuData.ay;
      rc.dataFrame.mcb.imuData[2] = imuData.az;
      rc.dataFrame.mcb.imuData[3] = imuData.gx;
      rc.dataFrame.mcb.imuData[4] = imuData.gy;
      rc.dataFrame.mcb.imuData[5] = imuData.gz;
      rc.dataFrame.mcb.imuData[6] = imuData.mx;
      rc.dataFrame.mcb.imuData[7] = imuData.my;
      rc.dataFrame.mcb.imuData[8] = imuData.mz;
      rc.dataFrame.mcb.imuData[9] = imuData.temperature;
      rc.dataFrame.mcb.imuData[10] = imuData.pressure;
      rc.dataFrame.mcb.altitude = imuData.altitude;
      
      //LP26HB - pressure
      rc.dataFrame.mcb.pressure = pressureSensor.getPressure_hPa();
      rc.dataFrame.mcb.temp_lp25 = pressureSensor.getTemperature_degC();
      
      //MCP temp
      rc.dataFrame.mcb.temp_mcp = tempsensor.readTempC();

      // Recovery:
      
      xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
      rc.recoveryStm.getRecoveryData((uint8_t*) &rc.dataFrame.recovery);
      xSemaphoreGive(rc.hardware.i2c1Mutex);
      
    /**********************/
      //calculation 
      
      //change state to first stage revcovery after 1 recov deploy
      if(SM_getCurrentState() == FLIGHT && rc.dataFrame.recovery.firstStageDone == true){
        SM_changeStateRequest(FIRST_STAGE_RECOVERY);
        rc.sendLog("First stage recovery");
      //change state to first stage revcovery after 2 recov deploy
      }else if(SM_getCurrentState() == FIRST_STAGE_RECOVERY && rc.dataFrame.recovery.secondStageDone == true){
        SM_changeStateRequest(SECOND_STAGE_RECOVERY);
        rc.sendLog("Second stage recovery");
      }
     
      //detect apogee
      if(SM_getCurrentState() == States::FLIGHT && rc.dataFrame.mcb.apogee == 0){
        if(lastMaxAltitude < imuData.altitude || lastMaxAltitude == 0){
          lastMaxAltitude = imuData.altitude;
          apogeeConfirmationTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        }

        if((xTaskGetTickCount() * portTICK_PERIOD_MS - apogeeConfirmationTimer) > 500){
          rc.dataFrame.mcb.apogee = lastMaxAltitude;
          sprintf(sd, "Apoge detected! Time %d, Altitude %f", rc.missionTimer.getTime(), rc.dataFrame.mcb.apogee);
          rc.sendLog(sd);
        }
      }

      //detect landing
      if(SM_getCurrentState() == States::SECOND_STAGE_RECOVERY){
        if(imuData.altitude < (launchPadAltitude + 50)){
          SM_changeStateRequest(States::ON_GROUND);
        }
      }
      
    }
    
    //LORA
    if(((xTaskGetTickCount() * portTICK_PERIOD_MS - loraTimer) >= rc.options.loraCurrentPeriod) || ulTaskNotifyTake(pdTRUE, 0)){
      loraTimer = xTaskGetTickCount() * portTICK_PERIOD_MS; //reset timer
      
      rc.dataFrame.mcb.state = SM_getCurrentState(); //get the newest information about state
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
    if((SM_getCurrentState() > COUNTDOWN && SM_getCurrentState() < ON_GROUND)){
      if((xTaskGetTickCount() * portTICK_RATE_MS - flashTimer) >= rc.options.flashDataCurrentPeriod){
        flashTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
        rc.dataFrame.mcb.state = SM_getCurrentState(); //get the newest information about state
        rc.dataFrame.missionTimer = rc.missionTimer.getTime(); //get mission time

        if(rc.options.flashWrite){
          if(xQueueSend(rc.hardware.flashQueue, (void*)&rc.dataFrame, 0) != pdTRUE){
            rc.errors.setRTOSError(RTOS_FLASH_QUEUE_ADD_ERROR);
            rc.sendLog("Flash queue is full");
          }
        }
        Serial.print("Data frame sizeof: ");
        Serial.println(sizeof(DataFrame));
        if(esp_now_send(adressBlackBox, (uint8_t*) &rc.dataFrame, sizeof(DataFrame)) != ESP_OK){
          rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          rc.sendLog("Black box send error");
        }
      }
        
      
    }
    

    //SD
    if((xTaskGetTickCount() * portTICK_RATE_MS - sdTimer) >= rc.options.sdDataCurrentPeriod){
      sdTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      
      rc.dataFrame.mcb.state = SM_getCurrentState(); //get the newest information about state
      rc.createSDFrame(sd);
      

      if(xQueueSend(rc.hardware.sdQueue, (void*)&sd, 0) != pdTRUE){ //data to SD
        rc.errors.setRTOSError(RTOS_SD_QUEUE_ADD_ERROR);
      }  
      
      rc.errors.reset(ERROR_RESET_SD); //reset errors after save  
    }

    vTaskDelay(10/portTICK_PERIOD_MS);
 
  }
}