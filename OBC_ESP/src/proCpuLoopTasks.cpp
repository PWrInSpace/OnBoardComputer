#include "mainStructs.h"
#include "timers.h"
#include "tasksAndTimers.h"

extern RocketControl rc;
extern WatchdogTimer wt;


void loraTask(void *arg){
  String loraRx;
  String loraTx;

  while(1){
    //Serial.println("LoRa TASK"); //DEBUG

    //if lora.available()


    if(xQueueReceive(rc.loraTxQueue, (void*)&loraTx, 25) == pdTRUE){
      //lora send
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}



void rxHandlingTask(void *arg){
  String loraData;
  uint8_t unit;

  while(1){
    //Serial.println("RX handling tasks"); //DEBUG

    if(xQueueReceive(rc.loraRxQueue, (void*)&loraData, 25) == pdTRUE){
      if(loraData.startsWith("1234")){

      }
    }

    if(xQueueReceive(rc.espNowQueue, (void*)&unit, 25)){
      switch(unit){
        case TANWA:
          //TanWa
          Serial.println("TanWa notify"); //DEBUG
          break;
        case PITOT:
          //pitot
          Serial.println("Pitot notify"); //DEBUG
          break;
        case MAIN_VALVE:
          //mainValve
          Serial.println("MainValve notify"); //DEBUG
          break;
        case UPUST_VALVE:
          //upustValve
          Serial.println("UpustValve notify"); //DEBUG
          break;
        case BLACK_BOX:
          //blackBox
          Serial.println("Black Box notify"); //DEBUG
          break;
        default:
          //TODO log error
          break;
      }
    }

    wt.rxHandlingTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}