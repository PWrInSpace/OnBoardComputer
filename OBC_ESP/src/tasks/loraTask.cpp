#include "../include/tasks/tasks.h"


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