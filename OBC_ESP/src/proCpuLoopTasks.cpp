#include "tasksAndTimers.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;

void loraTask(void *arg){
  String loraRx;
  String loraTx;

  while(1){
    //DEBUG GIGA
    if(Serial.available()){
      int x = Serial.readStringUntil('\n').toInt();
      if(x == 0){
        ESP.restart();
      }else if(!rc.changeStateEvent((StateMachineEvent)x)){
        dataFrame.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
      }
    }
    //DEBUG GIGA
    
    if(xSemaphoreTake(rc.spiMutex, 10) == pdTRUE){
      //if lora.available()

      xSemaphoreGive(rc.spiMutex);
    }else{
      dataFrame.errors.setRTOSError(RTOS_SPI_MUTEX_ERROR);
      Serial.println("MUTEX ERROR");//DEBUG
    }

    if(xQueueReceive(rc.loraTxQueue, (void*)&loraTx, 0) == pdTRUE){
      //Serial.println(loraTx); //DEBUG
      if(xSemaphoreTake(rc.spiMutex, 10) == pdTRUE){
        //LORA SEND
        xSemaphoreGive(rc.spiMutex);
      }else{
        dataFrame.errors.setRTOSError(RTOS_SPI_MUTEX_ERROR);
        Serial.println("MUTEX ERROR"); //DEBUG
      }
    }

    wt.loraTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

/**********************************************************************************************/

void rxHandlingTask(void *arg){
  String loraData;
  uint8_t rxEspNumber = 0;

  while(1){
    //Serial.println("RX handling tasks"); //DEBUG
    
    if(xQueueReceive(rc.loraRxQueue, (void*)&loraData, 25) == pdTRUE){
      if(loraData.startsWith("1234")){

      }
    }

    if(xQueueReceive(rc.espNowQueue, (void*) &rxEspNumber, 25)){

      TxDataEspNow txDataEspNow;

      switch(rxEspNumber){
        case TANWA:
          //TanWa
          Serial.println("TanWa notify"); //DEBUG
          break;

        case PITOT:
          Serial.println("Pitot notify"); //DEBUG
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) txDataEspNow.sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = rc.options.espnowFastPeriod;
          else txDataEspNow.sleepTime = rc.options.espnowSlowPeriod;

          esp_now_send(adressPitot, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          break;

        case MAIN_VALVE:
          Serial.println("MainValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) txDataEspNow.sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = rc.options.espnowFastPeriod;
          else txDataEspNow.sleepTime = rc.options.espnowSlowPeriod;

          //DEBUG GIGA
          Serial.println("Main valve data");
          Serial.print("wake up: "); Serial.println(dataFrame.mainValve.wakeUp);
          Serial.print("valve state: "); Serial.println(dataFrame.mainValve.valveState);
          Serial.print("thermocuple 0: "); Serial.println(dataFrame.mainValve.thermocouple[0]);
          Serial.print("thermocuple 1: "); Serial.println(dataFrame.mainValve.thermocouple[1]);
          Serial.print("batt : "); Serial.println(dataFrame.mainValve.batteryVoltage);
          //DEBUG GIGA

          txDataEspNow.command      = rc.options.mainValveRequestState;
          txDataEspNow.commandTime  = rc.options.mainValveCommandTime;
          
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          break;

        case UPUST_VALVE:
          Serial.println("UpustValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) txDataEspNow.sleepTime = rc.options.espnowSleepTime;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = rc.options.espnowFastPeriod;
          else txDataEspNow.sleepTime = rc.options.espnowSlowPeriod;
          
          txDataEspNow.command      = rc.options.upustValveRequestState;
          txDataEspNow.commandTime  = rc.options.upustValveCommandTime;
          
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          break;

        case BLACK_BOX:
          //blackBox TODO wymyśleć co i jak.
          Serial.println("Black Box notify"); //DEBUG
          
          break;

        default:
          rc.sendLog("Unknown esp now device: " + String(rxEspNumber));
          break;
      }
    }

    wt.rxHandlingTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}