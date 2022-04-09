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
    // To mi wywalało ESP, podczas odłączania seriala :D
    /*if(Serial.available()){
      int x = Serial.readStringUntil('\n').toInt();
      if(x == 0){
        ESP.restart();
      }else if(!rc.changeStateEvent((StateMachineEvent)x)){
        dataFrame.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
      }
    }*/
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
        parseR4A(&loraData[4]);

      }else if(strncmp(loraData, "R4O", 3) == 0){
        parseR4O(&loraData[4]);
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
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

          esp_now_send(adressPitot, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case MAIN_VALVE:
          Serial.println("MainValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;
          
          esp_now_send(adressMValve, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case UPUST_VALVE:
          Serial.println("UpustValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state == ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;
          
          esp_now_send(adressUpust, (uint8_t*) &sleepTime, sizeof(sleepTime));
          
          break;

        case BLACK_BOX:
          Serial.println("Black Box notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

          esp_now_send(adressBlackBox, (uint8_t*) &sleepTime, sizeof(sleepTime));

          break;

        case PAYLOAD:
          Serial.println("Payload notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

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

/**********************************************************************************************/

void parseR4A(char* data) {

  if (strstr(data, "STAT;")) {
    int oldState;
    int newState;

    sscanf(data, "STAT;%d;%d", &oldState, &newState);
    if (oldState == dataFrame.state && newState != StateMachine::FLIGHT) rc.changeStateEvent((StateMachineEvent) newState);
  }

  else if (strstr(data, "ABRT;")) {
    rc.changeState(ABORT);
  }
}

/**********************************************************************************************/

void parseR4O(char* data) {
  char callback[LORA_FRAME_ARRAY_SIZE];
  // Options:
  if (strstr(data, "OPTS;")) {
    int optionNumber;
    int optionValue;

    sscanf(data, "OPTS;%d;%d", &optionNumber, &optionValue);

    switch (optionNumber) {
    case 1:

      rc.options.LoRaFrequencyMHz = optionValue;
      xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
      LoRa.setFrequency((int)rc.options.LoRaFrequencyMHz * 1E6);
      xSemaphoreGive(rc.spiMutex);
      break;
    
    case 2: rc.options.countdownTime    = optionValue; break;
    case 3: rc.options.ignitionTime     = optionValue; break;    
    case 4: rc.options.tankMinPressure  = optionValue; break;
    case 5: rc.options.flashWrite       = optionValue; break;
    case 6: rc.options.forceLaunch      = optionValue; break;
    case 15: rc.options.dataFramePeriod = optionValue; break;
    case 16: rc.options.loraPeriod      = optionValue; break;
    
    default:
      Serial.printf("Wrong LoRa command!!!"); // DEBUG
      break;
    }

    dataFrame.createLoRaOptionsFrame(rc.options, callback);
    xQueueSend(rc.loraTxQueue, (void*)callback, 0);
  }

  // Valves:
  else if (strstr(data, "MAIN;")) {
    
    TxDataEspNow txDataEspNow;
    sscanf(data, "MAIN;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
    esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
  }

  else if (strstr(data, "UPST;")) {
    
    TxDataEspNow txDataEspNow;
    sscanf(data, "UPST;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
    esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
  }

  else if (strstr(data, "WKUP")) {
    // TODO budzonko
  }
}