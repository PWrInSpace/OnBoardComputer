#include "../include/tasks/tasks.h"

void rxHandlingTask(void *arg){
  char loraData[LORA_FRAME_ARRAY_SIZE] = {};
  //char log[SD_FRAME_ARRAY_SIZE] = {};
  uint8_t rxEspNumber = 0;
  int oldState = 0;
  int newState = 0;
  char callback[LORA_FRAME_ARRAY_SIZE];

  while(1){
    //Serial.println("RX handling tasks"); //DEBUG
    
    if(xQueueReceive(rc.hardware.loraRxQueue, (void*)&loraData, 25) == pdTRUE){
      Serial.println(loraData);
      /*** R4A ***/
      if(strncmp(loraData, "R4A", 3) == 0){
        if (strstr(loraData, "STAT;") != NULL) {

          sscanf(loraData, "R4A;STAT;%d;%d", &oldState, &newState);
          if (oldState == StateMachine::getCurrentState() && newState != States::FLIGHT){
          
            if(StateMachine::changeStateRequest((States) newState) == false){
              rc.sendLog("invalid state change request");
              rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
            }
          }
        
        }else if (strstr(loraData, "ABRT") != NULL) {
          if(StateMachine::changeStateRequest(States::ABORT) == false){
            rc.sendLog("invalid ABORT request");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }

        }else if (strstr(loraData, "HOLD") != NULL){
          if(StateMachine::changeStateRequest(States::HOLD) == false){
            rc.sendLog("invalid ABORT request");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }

        }else{
          strcat(loraData, " - INVALID COMMAND");
          rc.sendLog(loraData);
          rc.errors.setLastException(INVALID_PREFIX_EXCEPTION);
        }
      
      /*** R4O ***/
      }else if(strncmp(loraData, "R4O", 3) == 0){
        // Options:
        if (strstr(loraData, "OPTS;") != NULL) {
          int optionNumber;
          int optionValue;
          
          sscanf(loraData, "R4O;OPTS;%d;%d", &optionNumber, &optionValue);

          switch (optionNumber) {
          case 1:

            rc.options.LoRaFrequencyMHz = optionValue;
            xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
            LoRa.setFrequency((int)rc.options.LoRaFrequencyMHz * 1E6);
            xSemaphoreGive(rc.hardware.spiMutex);
            break;
          
          case 2: 
            if(optionValue >= 10000){
              rc.options.countdownTime = optionValue; 
              rc.missionTimer.setDisableValue(rc.options.countdownTime * -1);
            }else{
              rc.sendLog("Invalid countdown time set");
              rc.errors.setLastException(INVALID_OPTION_VALUE);
            }
          
            break;
          
          case 3: 
            if(optionValue < 0 && abs(optionValue) < rc.options.countdownTime){
              rc.options.ignitionTime = optionValue; 
            }else{
              rc.sendLog("Invalid ignition time set");
              rc.errors.setLastException(INVALID_OPTION_VALUE);
            }
          
            break;    
          
          case 4: rc.options.tankMinPressure  = optionValue; break;
          case 5: rc.options.flashWrite       = optionValue; break;
          case 6: rc.options.forceLaunch      = optionValue; break;
          case 15: rc.options.dataFramePeriod = optionValue; break;
          case 16: rc.options.loraPeriod      = optionValue; break;
          
          default:
            rc.sendLog("Invalid option NUMBER");
            rc.errors.setLastException(INVALID_OPTION_NUMBER);
            break;
          }

          rc.createOptionsFrame(callback);
          xQueueSend(rc.hardware.loraTxQueue, (void*)callback, 0);
        }

        // Valves:1
        else if (strstr(loraData, "MVAL;") != NULL) {
          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;MVAL;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
          //DEBUG
          Serial.println("WYSYŁANIE KOMENDY DO MVAL");
          Serial.print("Command: "); Serial.println(txDataEspNow.command);
          Serial.print("Command time: "); Serial.println(txDataEspNow.commandTime);
          
          
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
        }

        else if (strstr(loraData, "UVAL;") != NULL) {
          
          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;UVAL;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
          
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
        }

        else if (strstr(loraData, "WKUP") != NULL) {
          // TODO budzonko
        }

        else if (strstr(loraData, "RECOV") != NULL) {

          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;RECOV;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);

          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.sendCommand(txDataEspNow.command, txDataEspNow.commandTime);
          xSemaphoreGive(rc.hardware.i2c1Mutex);
        }

        else if (strstr(loraData, "RST")){
          ESP.restart();
        }

        else if (strstr(loraData, "VOID")){
          rc.sendLog("VOID command appear");
        }

        else{
          strcat(loraData, " - INVALID COMMAND");
          rc.sendLog(loraData);
          rc.errors.setLastException(INVALID_PREFIX_EXCEPTION);
        }
      }else{
        strcat(loraData, " - INVALID PREFIX");
        rc.sendLog(loraData);
        rc.errors.setLastException(INVALID_PREFIX_EXCEPTION);
      }
    }

    if(xQueueReceive(rc.hardware.espNowQueue, (void*) &rxEspNumber, 25)){

      uint16_t sleepTime;

      switch(rxEspNumber){
        case TANWA:
          //TanWa
          Serial.println("TanWa notify"); //DEBUG
          break;

        case PITOT:
          Serial.println("Pitot notify"); //DEBUG
          if (StateMachine::getCurrentState() < COUNTDOWN || StateMachine::getCurrentState() >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (StateMachine::getCurrentState() == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

          if(esp_now_send(adressPitot, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          break;

        case MAIN_VALVE:
          Serial.println("MainValve notify"); //DEBUG
          if (StateMachine::getCurrentState() < FUELING || StateMachine::getCurrentState() >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (StateMachine::getCurrentState() == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;
          
          if(esp_now_send(adressMValve, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          break;

        case UPUST_VALVE:
          Serial.println("UpustValve notify"); //DEBUG
          if (StateMachine::getCurrentState() < FUELING || StateMachine::getCurrentState() == ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (StateMachine::getCurrentState() == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;
          
          if(esp_now_send(adressUpust, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          break;

        case BLACK_BOX:
          Serial.println("Black Box notify"); //DEBUG
          if (StateMachine::getCurrentState() < COUNTDOWN || StateMachine::getCurrentState() >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (StateMachine::getCurrentState() == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

          if(esp_now_send(adressBlackBox, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;

        case PAYLOAD:
          Serial.println("Payload notify"); //DEBUG
          if (StateMachine::getCurrentState() < COUNTDOWN || StateMachine::getCurrentState() >= ON_GROUND) sleepTime = rc.options.espnowSleepTime;
          else if (StateMachine::getCurrentState() == FLIGHT) sleepTime = rc.options.espnowShortPeriod;
          else sleepTime = rc.options.espnowLongPeriod;

          if(esp_now_send(adressPayLoad, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;

        default:
          rc.sendLog("Unknown esp now device");
          break;
      }
    }

    wt.rxHandlingTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
  }
}
