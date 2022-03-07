#include "tasksAndTimers.h"

extern RocketControl rc;
extern WatchdogTimer wt;
extern DataFrame dataFrame;

void loraTask(void *arg){
  String loraRx;
  String loraTx;

  while(1){
    //Serial.println("LoRa TASK"); //DEBUG

    //if lora.available()
    //DEBUG GIGA
    if(Serial.available()){
      int x = Serial.readStringUntil('\n').toInt();
      Serial.print("DEBUG: "); Serial.println(x);
      rc.changeStateEvent((StateMachineEvent)x);
    }
    //DEBUG GIGA

    if(xQueueReceive(rc.loraTxQueue, (void*)&loraTx, 25) == pdTRUE){
      //lora send
    }

    wt.loraTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
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
          if (rc.state < COUNTDOWN || rc.state >= ON_GROUND) txDataEspNow.sleepTime = 30000;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = 100;
          else txDataEspNow.sleepTime = 500;

          esp_now_send(adressPitot, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          break;

        case MAIN_VALVE:
          Serial.println("MainValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) txDataEspNow.sleepTime = 30000;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = 100;
          else txDataEspNow.sleepTime = 500;

          //DEBUG GIGA
          Serial.println("Main valve data");
          Serial.print("wake up: "); Serial.println(dataFrame.mainValve.wakeUp);
          Serial.print("valve state: "); Serial.println(dataFrame.mainValve.valveState);
          Serial.print("thermocuple 0: "); Serial.println(dataFrame.mainValve.thermocouple[0]);
          Serial.print("thermocuple 1: "); Serial.println(dataFrame.mainValve.thermocouple[1]);
          Serial.print("batt : "); Serial.println(dataFrame.mainValve.batteryVoltage);
          //DEBUG GIGA

          txDataEspNow.command      = rc.options.mainValveRequestState;
          txDataEspNow.commandTime  = rc.options.mainValveTime;
          
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          break;

        case UPUST_VALVE:
          Serial.println("UpustValve notify"); //DEBUG
          if (rc.state < FUELING || rc.state >= ON_GROUND) txDataEspNow.sleepTime = 30000;
          else if (rc.state == FLIGHT) txDataEspNow.sleepTime = 100;
          else txDataEspNow.sleepTime = 500;
          
          txDataEspNow.command      = rc.options.upustValveRequestState;
          txDataEspNow.commandTime  = rc.options.upustValveTime;
          
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