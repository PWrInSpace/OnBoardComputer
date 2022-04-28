#include "../include/timers/watchdog.h"
#include "../include/structs/rocketControl.h"
#include "../include/structs/dataStructs.h"
#include "../include/tasks/tasks.h"
#include "../include/com/now.h"

WatchdogTimer wt;
RocketControl rc;


void setup() {
  Serial.begin(115200); //DEBUG
  Serial.print("Setup state: "); //DEBUG
  Serial.println(StateMachine::getCurrentState()); //DEBUG

  //set esp now

  nowInit();
  nowAddPeer(adressPitot, 0);
  nowAddPeer(adressMValve, 0);
  nowAddPeer(adressUpust, 0);
  nowAddPeer(adressBlackBox, 0);
  nowAddPeer(adressTanWa, 0);

  //init all components
  rc.hardware.i2c1.begin(I2C1_SDA, I2C1_SCL, 100E3);
  rc.hardware.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);
  rc.hardware.i2c1.setTimeOut(20);
  rc.hardware.i2c2.setTimeOut(20);

  //create Queues and Mutex //TODO
  rc.hardware.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.hardware.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(char[SD_FRAME_ARRAY_SIZE]));
  rc.hardware.flashQueue = xQueueCreate(FLASH_QUEUE_LENGTH, sizeof(DataFrame));
  rc.hardware.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  rc.hardware.spiMutex = xSemaphoreCreateMutex();
  rc.hardware.i2c1Mutex = xSemaphoreCreateMutex();

  //create Tasks
  //pro cpu
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 2, &rc.hardware.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &rc.hardware.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 5, &rc.hardware.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  30000, NULL, 2, &rc.hardware.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    30000, NULL, 3, &rc.hardware.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &rc.hardware.flashTask, APP_CPU_NUM);

  //create Timers
  rc.hardware.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

  rc.hardware.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
  
  //check created elements
  /*
  if(hm.loraRxQueue == NULL || hm.loraTxQueue == NULL || hm.sdQueue == NULL || hm.flashQueue == NULL || hm.espNowQueue == NULL){
    //error handling
    Serial.println("Queue create error!"); //DEBUG
    ESP.restart();
  }

  if(hm.spiMutex == NULL){
    //error handling
    Serial.println("Mutex create error!"); //DEBUG
    ESP.restart();
  }

  if(hm.loraTask == NULL || hm.rxHandlingTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(hm.stateTask == NULL || hm.dataTask == NULL || hm.sdTask == NULL || hm.flashTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(hm.watchdogTimer == NULL || hm.disconnectTimer == NULL){
    //error handling
    Serial.println("timer create error!"); //DEBUG
    ESP.restart();
  }
  */
  
  //watchdogtimer
  wt.begin();
  wt.EEPROMread();
  Serial.println(wt.previousState); //DEBUG
  Serial.println((uint8_t) wt.resetCounter); //DEBUG
  
  if(wt.previousState != INIT && wt.previousState != COUNTDOWN){
    StateMachine::changeStateRequest((States) wt.previousState);
  }else{
    StateMachine::changeStateRequest(States::IDLE);
  }

  //start timers
  xTimerStart(rc.hardware.watchdogTimer, portMAX_DELAY);
  xTimerStart(rc.hardware.disconnectTimer, portMAX_DELAY);

  vTaskDelete(NULL); //delete main task (loop())
}

void loop() {
}