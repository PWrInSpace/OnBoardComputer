#include "../include/tasks/tasks.h"

static void write_header_to_sd(SDCard &sd, String path) {
  char buffer[2000];
  pysd_create_header(buffer, sizeof(buffer), true);
  sd.write(path, buffer);
}

void sdTask(void *arg){
  SDCard mySD(rc.hardware.mySPI, SD_CS);
  char data[SD_FRAME_ARRAY_SIZE] = {};
  String dataPath = dataFileName;
  String logPath = logFileName;
  uint16_t sd_i = 0;
  bool sdError = false;

  vTaskDelay(50 / portTICK_RATE_MS);

  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);
  Serial.println("Semaphore take sd task");
  while(!mySD.init()){
    ERR_set_sd_error(SD_INIT_ERROR);
    Serial.println("SD INIT ERROR!"); //DEBUG

    xSemaphoreGive(rc.hardware.spiMutex);

    vTaskDelay(10000 / portTICK_PERIOD_MS);

    xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);
  }
  Serial.println("SD card initialized");

  while(mySD.fileExists(dataPath + String(sd_i) + ".txt")){
    sd_i++;
  }
  dataPath = dataPath + String(sd_i) + ".txt";
  logPath = logPath + String(sd_i) + ".txt";

  write_header_to_sd(mySD, dataPath);

  xSemaphoreGive(rc.hardware.spiMutex);

  uint32_t time = xTaskGetTickCount();

  while(1){

    if (xSemaphoreTake(rc.hardware.spiMutex, 50) == pdTRUE) {
      // Serial.println("Semaphore take sd task");
      if(xQueueReceive(rc.hardware.sdQueue, (void*)&data, 0) == pdTRUE){
          if(strncmp(data, "LOG", 3) == 0){
            if(!mySD.write(logPath, data)){
              ERR_set_sd_error(SD_WRITE_ERROR);
              sdError = true;
            }else{
              ERR_set_sd_error(SD_NO_ERROR);
              sdError = false;
            }
          }else{
            if(!mySD.write(dataPath, data)){
              ERR_set_sd_error(SD_WRITE_ERROR);
              sdError = true;
            }else{
              ERR_set_sd_error(SD_NO_ERROR);
              sdError = false;
            }
          }

      }
      xSemaphoreGive(rc.hardware.spiMutex);
      // Serial.println("Semaphore give sd task");
    }

    //FIX temporary
    if(sdError){
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }


    // if (xTaskGetTickCount() - time > 1000) {
    //   time = xTaskGetTickCount();
    //   Serial.println("SD task tick");
    // }

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}