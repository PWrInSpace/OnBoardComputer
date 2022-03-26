#include "tasksAndTimers.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;
extern SPIClass mySPI;

void loraTask(void *arg){
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2] = {};
  char loraTx[LORA_FRAME_ARRAY_SIZE] = {};

  xSemaphoreTake(rc.spiMutex, pdTRUE);

  LoRa.setSPI(mySPI);
  LoRa.setPins(4, 2, 17);
  LoRa.setSignalBandwidth(250E3);
  LoRa.noCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(14);
  LoRa.setTimeout(10);
  LoRa.begin(rc.options.LoRaFrequencyMHz * 1E6);

  xSemaphoreGive(rc.spiMutex);

  while(1){
    //DEBUG GIGA
    
    if(Serial.available()){
      int x = Serial.readStringUntil('\n').toInt();
      if(x == 0){
        ESP.restart();
      }else if(!rc.changeStateEvent((StateMachineEvent)x)){
        dataFrame.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
      }
    }
    //DEBUG GIGA

    xSemaphoreTake(rc.spiMutex, portMAX_DELAY);

      LoRa.parsePacket();
      if (LoRa.available()) {

        String rxStr = LoRa.readString();
        Serial.print(rxStr); // DEBUG

        strcpy(loraRx, rxStr.c_str());
        xQueueSend(rc.loraRxQueue, (void*)&loraRx, 0);
      }

    xSemaphoreGive(rc.spiMutex);

    if(xQueueReceive(rc.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      Serial.print(loraTx); //DEBUG
      xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
        
        LoRa.beginPacket();
        LoRa.print((char*) loraTx);
        LoRa.endPacket();

      xSemaphoreGive(rc.spiMutex);
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}

/**********************************************************************************************/

void rxHandlingTask(void *arg){
  char loraData[LORA_FRAME_ARRAY_SIZE] = {};
  char log[SD_FRAME_ARRAY_SIZE] = {};
  uint8_t rxEspNumber = 0;

  while(1){
    //Serial.println("RX handling tasks"); //DEBUG
    
    if(xQueueReceive(rc.loraRxQueue, (void*)&loraData, 25) == pdTRUE){
      if(strncmp(loraData, "R4A", 3) == 0){

      }else if(strncmp(loraData, "R4O", 3) == 0){

      }
    }

    if(xQueueReceive(rc.espNowQueue, (void*) &rxEspNumber, 25)){

      uint16_t sleepTime;

      switch(rxEspNumber){
        case TANWA:
          //TanWa
          Serial.println("TanWa notify"); //DEBUG
          break;

        case PITOT:
          Serial.println("Pitot notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowFastPeriod;
          else sleepTime = rc.options.espnowSlowPeriod;

          esp_now_send(adressPitot, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case MAIN_VALVE:
          Serial.println("MainValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowFastPeriod;
          else sleepTime = rc.options.espnowSlowPeriod;

          //DEBUG GIGA
          Serial.println("Main valve data");
          Serial.print("wake up: "); Serial.println(dataFrame.mainValve.wakeUp);
          Serial.print("valve state: "); Serial.println(dataFrame.mainValve.valveState);
          Serial.print("thermocuple 0: "); Serial.println(dataFrame.mainValve.thermocouple[0]);
          Serial.print("thermocuple 1: "); Serial.println(dataFrame.mainValve.thermocouple[1]);
          Serial.print("batt : "); Serial.println(dataFrame.mainValve.batteryVoltage);
          //DEBUG GIGA
          
          esp_now_send(adressMValve, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case UPUST_VALVE:
          Serial.println("UpustValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowFastPeriod;
          else sleepTime = rc.options.espnowSlowPeriod;
          
          esp_now_send(adressUpust, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case BLACK_BOX:
          Serial.println("Black Box notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowFastPeriod;
          else sleepTime = rc.options.espnowSlowPeriod;

          esp_now_send(adressBlackBox, (uint8_t*) &sleepTime, sizeof(sleepTime));

          break;

        case PAYLOAD:
          Serial.println("Payload notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowFastPeriod;
          else sleepTime = rc.options.espnowSlowPeriod;

          esp_now_send(adressPayLoad, (uint8_t*) &sleepTime, sizeof(sleepTime));

          break;

        default:
          strcpy(log, "Unknown esp now device:");
          rc.sendLog(log);
          break;
      }
    }

    wt.rxHandlingTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}