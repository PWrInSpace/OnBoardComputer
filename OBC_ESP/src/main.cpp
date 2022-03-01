#include <Arduino.h>
#include <SPI.h>
#include "FreeRTOS.h"
#include "timers.h"
#include "mainStructs.h"
#include "dataStructs.h"
#include "tasksAndTimers.h"

WatchdogTimer wt;
RocketControl rc;
SPIClass mySPI(VSPI);

void setup() {
  Serial.begin(115200); //DEBUG
  //rc.state = INIT;
  Serial.print("Setup state: "); //DEBUG
  Serial.println(rc.state); //DEBUG

  //set esp now


  //init all components


  //create Queues and Mutex //TODO
  rc.loraRxQueue = xQueueCreate(LORA_RX_QUEUE_LENGTH, sizeof(String)); //idk String or char[256]
  rc.loraTxQueue = xQueueCreate(LORA_TX_QUEUE_LENGTH, sizeof(String)); //idk String or char[256]
  rc.sdQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(DataFrame));
  rc.flashQueue = xQueueCreate(SD_QUEUE_LENGTH, sizeof(DataFrame)); //idk dataStruct String or char
  rc.espNowQueue = xQueueCreate(ESP_NOW_QUEUE_LENGTH, sizeof(uint8_t));

  rc.spiMutex = xSemaphoreCreateMutex();


  //create Tasks
  //pro cpu
  xTaskCreatePinnedToCore(loraTask,       "LoRa task",        8192, NULL, 2, &rc.loraTask,       PRO_CPU_NUM);
  xTaskCreatePinnedToCore(rxHandlingTask, "RX handling task", 8192, NULL, 2, &rc.rxHandlingTask, PRO_CPU_NUM);

  //app cpu
  xTaskCreatePinnedToCore(stateTask, "State task", 8192, NULL, 3, &rc.stateTask, APP_CPU_NUM);
  xTaskCreatePinnedToCore(dataTask,  "Data task",  8192, NULL, 2, &rc.dataTask,  APP_CPU_NUM);
  xTaskCreatePinnedToCore(sdTask,    "SD task",    8192, NULL, 1, &rc.sdTask,    APP_CPU_NUM);
  xTaskCreatePinnedToCore(flashTask, "Flash task", 8192, NULL, 1, &rc.flashTask, APP_CPU_NUM);

  //create Timers
  rc.watchdogTimer = xTimerCreate("watchdog timer", watchdogDelay, pdTRUE, NULL, watchdogTimerCallback);
  rc.disconnectTimer = xTimerCreate("disconnect timer", disconnectDelay, pdFALSE, NULL, disconnectTimerCallback);

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
    
    //check wachdog timer previous state
  if(wt.previousState != INIT){
      rc.state = (StateMachine) wt.previousState;
  }else{
        rc.state = IDLE;
  }

    //start timers
  xTimerStart(rc.watchdogTimer, portMAX_DELAY);
  xTimerStart(rc.disconnectTimer, portMAX_DELAY);
    
  vTaskDelete(NULL); //delete main task (loop())
}

void loop() {

}