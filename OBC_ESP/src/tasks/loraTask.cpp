#include "../include/tasks/tasks.h"


void loraTask(void *arg){
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2] = {};
  char loraTx[LORA_FRAME_ARRAY_SIZE] = {};

  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  LoRa.setSPI(rc.hardware.mySPI);
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

  xSemaphoreGive(rc.hardware.spiMutex);

  while(1){
    xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);

      LoRa.parsePacket();
      if (LoRa.available()) {

        String rxStr = LoRa.readString();
        Serial.print(rxStr); // DEBUG

        strcpy(loraRx, rxStr.c_str());
        xQueueSend(rc.hardware.loraRxQueue, (void*)&loraRx, 0);
        xTimerReset(rc.hardware.disconnectTimer, 0);
      }

    xSemaphoreGive(rc.hardware.spiMutex);

    if(xQueueReceive(rc.hardware.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      //Serial.print("LORA: ");
      //Serial.print(loraTx); //DEBUG
      xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
        
        if(LoRa.beginPacket() == 0){
          Serial.println("LORA is transmitnig");
        }
        LoRa.write((uint8_t*) loraTx, strlen(loraTx));
        //Serial.print("LORA SEND: ");
        //Serial.println(digitalRead(LORA_D0));
        if(LoRa.endPacket() != 1){
          Serial.println("End packet error!");
        }

      xSemaphoreGive(rc.hardware.spiMutex);
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}