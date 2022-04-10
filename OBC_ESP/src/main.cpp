#include "timers.h"
#include "mainStructs.h"
#include "dataStructs.h"
#include "tasksAndTimers.h"
#include "now.h"

//TO DO LIST
// w lora rx nie dać możliwości przejścia ze stanu 5 na 6
// przemyśleć w jaki sposób zerować errory, prawdopodbnie mieszanka tych dwóch pomysłów
// -> po zapisie na sd wszystkie - moze nadal być ale nie zdąży wskoczyć przez jakieś opóźnienie
// -> ustawaić brak errora gdy jest gt -> Moim zdaniem to lepiej
// przy testach sprawdzać działanie spi mutex

WatchdogTimer wt;
RocketControl rc;

volatile DataFrame dataFrame;

void setup() {
  delay(300);
  Serial.begin(115200); //DEBUG
  Serial.print("Setup state: "); //DEBUG
  Serial.println(rc.state); //DEBUG

  //set esp now
  
  nowInit();
  nowAddPeer(adressPitot, 0);
  nowAddPeer(adressMValve, 0);
  nowAddPeer(adressUpust, 0);
  nowAddPeer(adressBlackBox, 0);
  nowAddPeer(adressTanWa, 0);

  //init all components
  rc.i2c1.begin(I2C1_SDA, I2C1_SCL, 100E3);
  rc.i2c2.begin(I2C2_SDA, I2C2_SCL, 100E3);
  rc.i2c1.setTimeOut(20);
  rc.i2c2.setTimeOut(20);

  //create Queues and Mutex //TODO
  rc.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(char[LORA_FRAME_ARRAY_SIZE]));
  rc.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(char[SD_FRAME_ARRAY_SIZE]));
  rc.flashQueue = xQueueCreate(FLASH_QUEUE_LENGTH, sizeof(DataFrame));
  rc.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  rc.spiMutex = xSemaphoreCreateMutex();


  //create Tasks
  //pro cpu
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 2, &rc.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &rc.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 5, &rc.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  30000, NULL, 2, &rc.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    30000, NULL, 3, &rc.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &rc.flashTask, APP_CPU_NUM);

  //create Timers
  rc.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

  rc.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
  
  //check created elements
  if(rc.loraRxQueue == NULL || rc.loraTxQueue == NULL || rc.sdQueue == NULL || rc.flashQueue == NULL || rc.espNowQueue == NULL){
    //error handling
    Serial.println("Queue create error!"); //DEBUG
    ESP.restart();
  }

  if(rc.spiMutex == NULL){
    //error handling
    Serial.println("Mutex create error!"); //DEBUG
    ESP.restart();
  }

  if(rc.loraTask == NULL || rc.rxHandlingTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(rc.stateTask == NULL || rc.dataTask == NULL || rc.sdTask == NULL || rc.flashTask == NULL){
    //error handling
    Serial.println("ProCPU task create error!"); //DEBUG
    ESP.restart();  
  }

  if(rc.watchdogTimer == NULL || rc.disconnectTimer == NULL){
    //error handling
    Serial.println("timer create error!"); //DEBUG
    ESP.restart();
  }

  
  //watchdogtimer
  wt.begin();
  wt.EEPROMread();
  Serial.println(wt.previousState); //DEBUG
  Serial.println((uint8_t) wt.resetCounter); //DEBUG
  dataFrame.watchdogResets = wt.resetCounter;
    //check wachdog timer previous state
  if(wt.previousState != INIT_EVENT && wt.previousState != COUNTDOWN_EVENT){
    rc.changeStateEvent((StateMachineEvent) wt.previousState);
  }else{
    rc.changeStateEvent(StateMachineEvent::IDLE_EVENT);
  }
/*
  //start timers
  xTimerStart(rc.watchdogTimer, portMAX_DELAY);
  xTimerStart(rc.disconnectTimer, portMAX_DELAY);
*/
  vTaskDelete(NULL); //delete main task (loop())
}

void loop() {
}