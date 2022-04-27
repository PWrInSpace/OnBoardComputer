#include "../include/timers/watchdog.h"
#include "../include/structs/mainStructs.h"
#include "../include/structs/dataStructs.h"
#include "../include/tasks/tasks.h"
#include "../include/com/now.h"

WatchdogTimer wt;
//RocketControl rc;
HardwareManagment hm;

void setup() {
  delay(300);
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
  hm.i2c1.begin(I2C1_SDA, I2C1_SCL, 100E3);
  hm.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);
  hm.i2c1.setTimeOut(20);
  hm.i2c2.setTimeOut(20);

  //create Queues and Mutex //TODO
  hm.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  hm.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  hm.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(char[SD_FRAME_ARRAY_SIZE]));
  hm.flashQueue = xQueueCreate(FLASH_QUEUE_LENGTH, sizeof(DataFrame));
  hm.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  hm.spiMutex = xSemaphoreCreateMutex();
  hm.i2c1Mutex = xSemaphoreCreateMutex();

  //create Tasks
  //pro cpu
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 2, &hm.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &hm.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 5, &hm.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  30000, NULL, 2, &hm.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    30000, NULL, 3, &hm.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &hm.flashTask, APP_CPU_NUM);

  //create Timers
  hm.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

  hm.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
  
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
  
  //check wachdog timer previous state
  if(wt.previousState != INIT && wt.previousState != COUNTDOWN){
    StateMachine::changeStateRequest((States) wt.previousState);
  }else{
    StateMachine::changeStateRequest(States::IDLE);
  }

  //start timers
  xTimerStart(hm.watchdogTimer, portMAX_DELAY);
  xTimerStart(hm.disconnectTimer, portMAX_DELAY);

  vTaskDelete(NULL); //delete main task (loop())
}

void loop() {
}