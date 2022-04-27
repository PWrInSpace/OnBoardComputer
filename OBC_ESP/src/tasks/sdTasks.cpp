#include "../include/tasks/tasks.h"

void sdTask(void *arg){
  SDCard mySD(rc.mySPI, SD_CS);
  char data[SD_FRAME_ARRAY_SIZE] = {};
  String dataPath = dataFileName;
  String logPath = logFileName;
  uint8_t sd_i = 0;

  vTaskDelay(50 / portTICK_RATE_MS);

  xSemaphoreTake(rc.spiMutex, pdTRUE);

  while(!mySD.init()){
    dataFrame.errors.setSDError(SD_INIT_ERROR);
    Serial.println("SD INIT ERROR!");
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }

  while(mySD.fileExists(dataPath + String(sd_i) + ".txt")){
    sd_i++;
  }
  dataPath = dataPath + String(sd_i) + ".txt";
  logPath = logPath + String(sd_i) + ".txt";

  xSemaphoreGive(rc.spiMutex);

  while(1){
    
    if(xQueueReceive(rc.sdQueue, (void*)&data, 0) == pdTRUE){
      
      xSemaphoreTake(rc.spiMutex, portMAX_DELAY);
      
        if(strncmp(data, "LOG", 3) == 0){
          if(!mySD.write(logPath, data)){
            dataFrame.errors.setSDError(SD_WRITE_ERROR);
          }
        }else{
          if(!mySD.write(dataPath, data)){
            dataFrame.errors.setSDError(SD_WRITE_ERROR);
          }
        }  

      xSemaphoreGive(rc.spiMutex);
    } 

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}