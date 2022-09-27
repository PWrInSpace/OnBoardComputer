#include "../include/tasks/tasks.h"

static struct {
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2];
  char loraTx[LORA_FRAME_ARRAY_SIZE];
}task = {
  .loraRx = {0},
  .loraTx = {0},
};

static bool lora_init(void) {
  LoRa.setSPI(rc.hardware.mySPI);
  LoRa.setPins(LORA_CS, LORA_RS, LORA_D0);
  
  if(LoRa.begin((int)OPT_get_lora_freq() * 1E3) == 0) {
    return false;
  }

  LoRa.setSignalBandwidth(250E3);
  LoRa.disableCrc();
  LoRa.setSpreadingFactor(7);
  LoRa.setTxPower(14);
  LoRa.setTimeout(10);

  return true;
}

static void lora_read_message_and_put_on_queue(void) {
  String rxStr = LoRa.readString();
  Serial.print(rxStr); // DEBUG

  if(rxStr.length() < (LORA_FRAME_ARRAY_SIZE/2 - 1)){
    strcpy(task.loraRx, rxStr.c_str());
    xQueueSend(rc.hardware.loraRxQueue, (void*)&task.loraRx, 0);
  }
}

static void lora_write_message(uint8_t *data, size_t size) {
  // idk why the ifs looks like this but i am afraid to delete this //TODO:
  if(LoRa.beginPacket() == 0);
  LoRa.write(data, size);
  if(LoRa.endPacket() != 1);
}

void loraTask(void *arg){
  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  if (lora_init() == false) {
    Serial.println("LORA init error!");
    ESP.restart();
  }

  xSemaphoreGive(rc.hardware.spiMutex);

  vTaskDelay(100 / portTICK_PERIOD_MS);

  while(1){
    xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);

    LoRa.parsePacket();
    if (LoRa.available()) {
      lora_read_message_and_put_on_queue();
    }

    xSemaphoreGive(rc.hardware.spiMutex);

    if(xQueueReceive(rc.hardware.loraTxQueue, (void*)&task.loraTx, 0) == pdTRUE){
      xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);

      lora_write_message((uint8_t*) task.loraTx, sizeof(task.loraTx));

      xSemaphoreGive(rc.hardware.spiMutex);
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}