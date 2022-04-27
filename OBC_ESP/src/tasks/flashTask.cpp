#include "../include/tasks/tasks.h"

void flashTask(void *arg){

  File file;
  DataFrame frame;
  LITTLEFS.begin(true);
  bool wipeFile = true;

  while(1){

    if (uxQueueMessagesWaiting(rc.flashQueue) > FLASH_QUEUE_LENGTH / 2) {

      // Wiping file for the first time to remove data from previous flights:
      if (wipeFile) {
        wipeFile = false;
        file = LITTLEFS.open("/file.txt", "w");
      }
      else file = LITTLEFS.open("/file.txt", "a");

      while (uxQueueMessagesWaiting(rc.flashQueue) > 0) {

        xQueueReceive(rc.flashQueue, &frame, portMAX_DELAY);
        file.write((uint8_t*) &frame, sizeof(frame));
      }

      file.close();
    }

    wt.flashTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}