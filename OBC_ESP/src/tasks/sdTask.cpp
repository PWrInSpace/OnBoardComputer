#include "../include/tasks/tasks.h"

void sdTask(void *arg){
  SDCard mySD(rc.hardware.mySPI, SD_CS);
  char data[SD_FRAME_ARRAY_SIZE] = {};
  String dataPath = dataFileName;
  String logPath = logFileName;
  uint8_t sd_i = 0;

  vTaskDelay(50 / portTICK_RATE_MS);

  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  while(!mySD.init()){
    rc.errors.setSDError(SD_INIT_ERROR);
    Serial.println("SD INIT ERROR!");
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  while(mySD.fileExists(dataPath + String(sd_i) + ".txt")){
    sd_i++;
  }
  dataPath = dataPath + String(sd_i) + ".txt";
  logPath = logPath + String(sd_i) + ".txt";

  xSemaphoreGive(rc.hardware.spiMutex);

  while(1){
    
    if(xQueueReceive(rc.hardware.sdQueue, (void*)&data, 0) == pdTRUE){
      
      xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
      
        if(strncmp(data, "LOG", 3) == 0){
          if(!mySD.write(logPath, data)){
            rc.errors.setSDError(SD_WRITE_ERROR);
          }
        }else{
          if(!mySD.write(dataPath, data)){
            rc.errors.setSDError(SD_WRITE_ERROR);
          }
        }  

      xSemaphoreGive(rc.hardware.spiMutex);
    } 

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}