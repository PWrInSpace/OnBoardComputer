#include "../include/tasks/tasks.h"

void flashTask(void *arg){

  File file;
  DataFrame frame;
  LITTLEFS.begin(true);
  bool wipeFile = true;

  while(1){

    if (uxQueueMessagesWaiting(rc.hardware.flashQueue) > FLASH_QUEUE_LENGTH / 2) {

      // Wiping file for the first time to remove data from previous flights:
      if (wipeFile) {
        wipeFile = false;
        file = LITTLEFS.open("/file.txt", "w");
      }
      else file = LITTLEFS.open("/file.txt", "a");

      while (uxQueueMessagesWaiting(rc.hardware.flashQueue) > 0) {

        xQueueReceive(rc.hardware.flashQueue, &frame, portMAX_DELAY);
        if(!file.write((uint8_t*) &frame, sizeof(frame))){
          rc.errors.setFlashError(FLASH_WRITE_ERROR);
        }
      }

      file.close();
    }

    wt.flashTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}