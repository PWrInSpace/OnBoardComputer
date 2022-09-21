#include "../include/tasks/tasks.h"

void sdTask(void *arg){
  SDCard mySD(rc.hardware.mySPI, SD_CS);
  char data[SD_FRAME_ARRAY_SIZE] = {};
  String dataPath = dataFileName;
  String logPath = logFileName;
  uint16_t sd_i = 0;
  bool sdError = false;

  vTaskDelay(50 / portTICK_RATE_MS);

  xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);

  while(!mySD.init()){
    ERR_set_sd_error(SD_INIT_ERROR);
    Serial.println("SD INIT ERROR!"); //DEBUG
    
    xSemaphoreGive(rc.hardware.spiMutex);
    
    vTaskDelay(10000 / portTICK_PERIOD_MS);

    xSemaphoreTake(rc.hardware.spiMutex, pdTRUE);
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

      xSemaphoreGive(rc.hardware.spiMutex);
    } 
    //FIX temporary
    if(sdError){
      vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    wt.sdTaskFlag = true;
    vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}