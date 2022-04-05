#include "tasksAndTimers.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;

void loraTask(void *arg){
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2] = {};
  char loraTx[LORA_FRAME_ARRAY_SIZE] = {};

  xSemaphoreTake(rc.spiMutex, pdTRUE);

  LoRa.setSPI(rc.mySPI);
  LoRa.setPins(LORA_CS, LORA_RS, LORA_D0);
  
  while(LoRa.begin((int)rc.options.LoRaFrequencyMHz * 1E6) == 0){
    Serial.println("LORA begin error!");
    vTaskDelay(500 / portTICK_PERIOD_MS);
  }

  LoRa.setSignalBandwidth(250E3);
  LoRa.disableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(14);
  LoRa.setTimeout(10);

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

      if(LoRa.parsePacket() != 0){
        Serial.println("Super parse 152");
      }
      //Serial.print("RSSI: ");
      //Serial.println(LoRa.rssi());
      //Serial.print("DO: ");
      //Serial.println(digitalRead(LORA_D0));
      //Serial.print("FREQ_ERROR: ");
      //Serial.println(LoRa.packetFrequencyError());
      if (LoRa.available()) {

        String rxStr = LoRa.readString();
        Serial.print(rxStr); // DEBUG

        strcpy(loraRx, rxStr.c_str());
        xQueueSend(rc.loraRxQueue, (void*)&loraRx, 0);
      }

    xSemaphoreGive(rc.spiMutex);

    if(xQueueReceive(rc.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      //Serial.print("LORA: ");
      //Serial.print(loraTx); //DEBUG
      xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
        
        if(LoRa.beginPacket() == 0){
          Serial.println("LORA is transmitnig");
        }
        char test[] = "Hello space!";
        LoRa.write((uint8_t*) loraTx, strlen(loraTx));
        //Serial.print("LORA SEND: ");
        //Serial.println(digitalRead(LORA_D0));
        if(LoRa.endPacket() != 1){
          Serial.println("End packet error!");
        }

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
        
        if(strstr(loraData, "R4O;OPTS;2")) {
          sscanf(loraData, "R4O;OPTS;2;%d", (int*) &rc.options.LoRaFrequencyMHz);
          
          xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
          LoRa.setFrequency((int)rc.options.LoRaFrequencyMHz * 1E6);
          xSemaphoreGive(rc.spiMutex);
        }
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