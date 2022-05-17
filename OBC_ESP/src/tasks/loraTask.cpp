#include "../include/tasks/tasks.h"


void loraTask(void *arg){
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2] = {};
  char loraTx[LORA_FRAME_ARRAY_SIZE] = {};

  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  LoRa.setSPI(rc.hardware.mySPI);
  LoRa.setPins(LORA_CS, LORA_RS, LORA_D0);
  
  while(LoRa.begin((int)rc.options.LoRaFrequencyMHz * 1E6) == 0){
    Serial.println("LORA begin error!");
    xSemaphoreGive(rc.hardware.spiMutex);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);
  }

  LoRa.setSignalBandwidth(250E3);
  LoRa.disableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(14);
  LoRa.setTimeout(10);
  xSemaphoreGive(rc.hardware.spiMutex);

  vTaskDelay(100 / portTICK_PERIOD_MS);

  while(1){
    xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);

      LoRa.parsePacket();
      if (LoRa.available()) {

        String rxStr = LoRa.readString();
        //Serial.print(rxStr); // DEBUG

        
        strcpy(loraRx, rxStr.c_str());
        xQueueSend(rc.hardware.loraRxQueue, (void*)&loraRx, 0);
        
        rc.restartDisconnectTimer(); 
      }
    xSemaphoreGive(rc.hardware.spiMutex); 


    if(xQueueReceive(rc.hardware.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
        
        if(LoRa.beginPacket() == 0);
        LoRa.write((uint8_t*) loraTx, strlen(loraTx));
        if(LoRa.endPacket() != 1);

      xSemaphoreGive(rc.hardware.spiMutex);
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}