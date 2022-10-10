#include "../include/tasks/tasks.h"

static struct {
  char loraRx[LORA_FRAME_ARRAY_SIZE / 2];
  char loraTx[LORA_FRAME_ARRAY_SIZE];
  bool data_to_send;
}task = {
  .loraRx = {0},
  .loraTx = {0},
  .data_to_send = false,
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
  String rxStr = "";
  uint32_t start_time = millis();
  uint32_t timeout = 250;
  uint32_t available = LoRa.available();
  rxStr.clear();

  while ((available > 0) && (millis() - start_time < timeout)) {
    rxStr += (char)LoRa.read();
    available -= 1;
  }

  if (millis() - start_time > timeout) {
    Serial.println("Lora timeout");
    return;
  }

  size_t rx_size = rxStr.length();

  Serial.print("Received: ");
  Serial.println(rxStr);
  Serial.print("Size: ");
  Serial.println(rx_size);

  if (rx_size < LORA_FRAME_ARRAY_SIZE - 1) {
    memcpy(task.loraRx, rxStr.c_str(), rx_size);
    task.loraRx[rx_size] = '\0';
    xQueueSend(rc.hardware.loraRxQueue, (void*)&task.loraRx, 0);
  }
}

static void lora_write_message(uint8_t *data, size_t size) {
  if(LoRa.beginPacket() != 0) {
    LoRa.write(data, size);
    LoRa.endPacket();
  }
}

void loraTask(void *arg){
  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  if (lora_init() == false) {
    Serial.println("LORA init error!");
    ESP.restart();
  }

  xSemaphoreGive(rc.hardware.spiMutex);

  vTaskDelay(100 / portTICK_PERIOD_MS);
  uint32_t time = xTaskGetTickCount();

  while(1){
    if (xSemaphoreTake(rc.hardware.spiMutex, 50) == pdTRUE) {
      int lora_size = LoRa.parsePacket();
      Serial.print("Parse packet size: ");
      Serial.println(lora_size);
      if (lora_size != 0) {
        int lora_available = LoRa.available();
        Serial.print("Lora available: ");
        Serial.println(lora_available);
        if (lora_available) {
          Serial.println("Lora read message");
          lora_read_message_and_put_on_queue();
        }
        memset(task.loraRx, 0, sizeof(task.loraRx));
      }
      xSemaphoreGive(rc.hardware.spiMutex);
    }

    vTaskDelay(100 / portTICK_PERIOD_MS);

    if (xSemaphoreTake(rc.hardware.spiMutex, 50) == pdTRUE) {
      // Serial.println("Semaphore take lora send");
      if(xQueueReceive(rc.hardware.loraTxQueue, (void*)&task.loraTx, 0) == pdTRUE){
        Serial.println("Lora write");
        lora_write_message((uint8_t*) task.loraTx, sizeof(task.loraTx));
        memset(task.loraTx, 0, sizeof(task.loraTx));
      }
      // Serial.println("Semaphore givelora send");
      xSemaphoreGive(rc.hardware.spiMutex);
    }

    wt.loraTaskFlag = true;
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}