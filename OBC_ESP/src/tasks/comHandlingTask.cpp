#include "../include/tasks/tasks.h"
#include "options.h"

void rxHandlingTask(void *arg){
  char loraData[LORA_FRAME_ARRAY_SIZE] = {};
  uint8_t rxEspNumber = 0;
  int oldState = 0;
  int newState = 0;
  char callback[LORA_FRAME_ARRAY_SIZE];

  while(1){
    //Serial.println("RX handling tasks"); //DEBUG
    
    if(xQueueReceive(rc.hardware.loraRxQueue, (void*)&loraData, 25) == pdTRUE){
      //Serial.println(loraData);
      /*** R4A ***/
      if(strncmp(loraData, "R4A", 3) == 0){
        rc.restartDisconnectTimer(); 

        if (strstr(loraData, "STAT;") != NULL) {

          sscanf(loraData, "R4A;STAT;%d;%d", &oldState, &newState);
          if (oldState == SM_getCurrentState() && newState != States::FLIGHT){
          
            if(SM_changeStateRequest((States) newState) == false){
              rc.sendLog("invalid state change request");
              rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
            }

          }else{
            rc.sendLog("invalid state change command");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }
        
        }else if (strstr(loraData, "ABRT") != NULL) {
          if(SM_changeStateRequest(States::ABORT) == false){
            rc.sendLog("invalid ABORT request");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }

        }else if (strstr(loraData, "HOLD_IN") != NULL){
          if(SM_changeStateRequest(States::HOLD) == false && SM_getCurrentState() != States::HOLD){
            rc.sendLog("invalid HOLD_IN request");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }

        }else if (strstr(loraData, "HOLD_OUT") != NULL){
          if(SM_changeStateRequest(States::HOLD) == false && SM_getCurrentState() == States::HOLD){
            rc.sendLog("invalid HOLD_OUT request");
            rc.errors.setLastException(INVALID_STATE_CHANGE_EXCEPTION);
          }

        }else{
          strcat(loraData, " - INVALID COMMAND");
          rc.sendLog(loraData);
          rc.errors.setLastException(INVALID_PREFIX_EXCEPTION);
        }
      
      /*** R4O ***/
      }else if(strncmp(loraData, "R4O", 3) == 0){
        rc.restartDisconnectTimer(); 
        
        // Options:
        if (strstr(loraData, "OPTS;") != NULL) {
          int optionNumber;
          int optionValue;
          
          sscanf(loraData, "R4O;OPTS;%d;%d", &optionNumber, &optionValue);

          switch (optionNumber) {
          case 1:

            if (OPT_set_lora_freq((uint32_t)optionValue) == true) {
              xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
              LoRa.setFrequency((int)OPT_get_lora_freq() * 1E3);
              xSemaphoreGive(rc.hardware.spiMutex);
            } else {
              rc.sendLog("Invalid lora frequency");
              rc.errors.setLastException(INVALID_OPTION_VALUE);
            }
            break;
          
          case 2: 
            if (OPT_set_countdown_begin_time(optionValue) == true) {
              rc.missionTimer.setDisableValue(OPT_get_countdown_begin_time());
            }else{
              rc.sendLog("Invalid countdown time set");
              rc.errors.setLastException(INVALID_OPTION_VALUE);
            }
            break;
          
          case 3: 
            if (OPT_set_ignition_time(optionValue) == false) {
              rc.sendLog("Invalid ignition time set");
              rc.errors.setLastException(INVALID_OPTION_VALUE);
            }
            break;    
          
          case 4: 
            OPT_set_tank_min_pressure(optionValue);
            break;
          
          case 5: 
            OPT_set_flash_write(optionValue);
            break;

          case 6: 
            OPT_set_force_launch(optionValue);
            break;

          case 7: 
            if(optionValue > 5 && optionValue < 60000){
              OPT_set_data_current_period(optionValue); 
            }else{
              rc.errors.setLastException(INVALID_OPTION_NUMBER);
            }
            break;
          
          case 8: 
            if(optionValue > 100 && optionValue < 60000){
              OPT_set_lora_current_period(optionValue);
            }else{
              rc.errors.setLastException(INVALID_OPTION_NUMBER);
            }
            break;

          case 9: 
            if(optionValue > 25 && optionValue < 60000){
              OPT_set_flash_write_current_period(optionValue);
            }else{
              rc.errors.setLastException(INVALID_OPTION_NUMBER);
            }
            break;

          case 10: 
            if(optionValue > 25 && optionValue < 60000){
              OPT_set_sd_write_current_period(optionValue);
            }else{
              rc.errors.setLastException(INVALID_OPTION_NUMBER);
            }
            break;

          default:
            rc.sendLog("Invalid option NUMBER");
            rc.errors.setLastException(INVALID_OPTION_NUMBER);
            break;
          }

          //send calback with new options
          rc.createOptionsFrame(callback, OPT_get_options_struct());
          xQueueSend(rc.hardware.loraTxQueue, (void*)callback, 0);
        }

        //Valves main
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

        // Valve upust
        else if (strstr(loraData, "UVAL;") != NULL) {
          
          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;UVAL;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
          
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
        }

        //Wake up
        else if (strstr(loraData, "WKUP") != NULL) {
          States tempState = SM_getCurrentState();
          
          while(tempState < States::COUNTDOWN){
            valvePeriod[tempState] = 500;
            pitotPeriod[tempState] = 500;
            espNowDefaultPeriod[tempState] = 500;
            tempState = static_cast<States>(tempState + 1);
          }
        }

        //go off
        else if (strstr(loraData, "GOFF") != NULL){
          States tempState = SM_getCurrentState();
          valvePeriod[tempState] = ESP_NOW_SLEEP_TIME;
          pitotPeriod[tempState] = ESP_NOW_SLEEP_TIME;
          espNowDefaultPeriod[tempState] = ESP_NOW_SLEEP_TIME;
        }

        //recovery
        else if (strstr(loraData, "RECOV") != NULL) {

          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;RECOV;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);

          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.sendCommand(txDataEspNow.command, txDataEspNow.commandTime);
          xSemaphoreGive(rc.hardware.i2c1Mutex);
        }

        else if(strstr(loraData, "TANWA") != NULL){
          TxDataEspNow txDataEspNow;
          sscanf(loraData, "R4O;TANWA;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
          
          if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
        }

        //reset
        else if (strstr(loraData, "RST")){
          ESP.restart();
        }

        //void command
        else if (strstr(loraData, "VOID")){
          rc.sendLog("VOID command appear");
        }

        //invalid command logger
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
      uint8_t currentState = SM_getCurrentState();
      if(currentState >= PERIOD_ARRAY_SIZE){
        rc.sendLog("Out of period array size :C");
        rxEspNumber = 0xff;
      }

      if(rxEspNumber < CONNECTION_CHECK_DEVICE_NUMBER){
        //Serial.print("Ustawiam: ");
        //Serial.println(rxEspNumber);
        rc.isConnectedFlags[rxEspNumber] = true;
      }

      switch(rxEspNumber){
        case TANWA:
          //TanWa
          Serial.println("TanWa notify"); //DEBUG
          break;

        case PITOT:
          Serial.println("Pitot notify"); //DEBUG
          sleepTime = pitotPeriod[currentState];
          
          if(esp_now_send(adressPitot, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            rc.isConnectedFlags[PITOT] = false;
          }
          
          break;

        case MAIN_VALVE:
          //Serial.println("MainValve notify"); //DEBUG
          sleepTime = valvePeriod[currentState];
          
          if(esp_now_send(adressMValve, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            rc.isConnectedFlags[MAIN_VALVE] = false;
          }
          
          break;

        case UPUST_VALVE:
          //Serial.println("UpustValve notify"); //DEBUG
          sleepTime = valvePeriod[currentState];
          
          if(esp_now_send(adressUpust, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          break;

        case BLACK_BOX:
          Serial.println("Black Box notify"); //DEBUG
          sleepTime = espNowDefaultPeriod[currentState];

          if(esp_now_send(adressBlackBox, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;

        case PAYLOAD:
          Serial.println("Payload notify"); //DEBUG
          sleepTime = payloadPeriod[currentState];
          
          if(esp_now_send(adressPayLoad, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;

        case ACS:
          Serial.println("ACS notify"); //DEBUG
          sleepTime = espNowDefaultPeriod[currentState];
          
          if(esp_now_send(adressACS, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;
        case CAMERA_ACS:
          Serial.println("camera ACS notify"); //DEBUG
          sleepTime = espNowDefaultPeriod[currentState];
          
          if(esp_now_send(cameraACS, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
         
          break;

        case CAMERA_PITOT:
          Serial.println("Camera pitot notify"); //DEBUG
          sleepTime = espNowDefaultPeriod[currentState];
          
          if(esp_now_send(cameraPitot, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;
        case CAMERA_RECOVERY:
          Serial.println("Camera recovery"); //DEBUG
          sleepTime = espNowDefaultPeriod[currentState];
          
          if(esp_now_send(cameraRecovery, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          break;
        case 0xFF:
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
