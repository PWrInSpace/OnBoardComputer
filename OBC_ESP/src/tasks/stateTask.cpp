#include "../include/tasks/tasks.h"

void stateTask(void *arg){
  StateMachine stateMachine(rc.hardware.stateTask);
  TxDataEspNow txDataEspNow;
  TickType_t stateChangeTimeMark = 0;
  TickType_t loopTimer = 0;
  
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 0)){
      //portENTER_CRITICAL(&rc.stateLock);
      //setup
      
      switch(stateMachine.getRequestedState()){
        case IDLE:
          rc.options.dataCurrentPeriod = DATA_PERIOD;
          stateMachine.changeStateConfirmation();
          break;

        case ARMED:
          //recovery arm request
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(true);
          vTaskDelay(150);
          rc.recoveryStm.setTelemetrum(true);
          xSemaphoreGive(rc.hardware.i2c1Mutex);
          //check recovery arm answer
          
          stateMachine.changeStateConfirmation();
          break;

        case FUELING:
          //TODO in future, check valves state
          
          txDataEspNow.setVal(VALVE_CLOSE, 0);
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
            
          stateMachine.changeStateConfirmation();
          break;

        case RDY_TO_LAUNCH:
          digitalWrite(CAMERA, HIGH); //turn on camera
          
          stateMachine.changeStateConfirmation();
          break;

        case COUNTDOWN:
          //dataframe 
          if(rc.allDevicesWokenUp() || rc.options.forceLaunch == true){
          
            //turn on mission timer
            rc.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(rc.missionTimer.isEnable()){
              if(rc.deactiveDisconnectTimer() == false){
                rc.sendLog("Timer delete error");
              } //turn off disconnectTimer

              stateMachine.changeStateConfirmation();
            }else{
              rc.errors.setLastException(MISSION_TIMER_EXCEPTION);
              stateMachine.changeStateRejection();
            }
          }else{
            rc.errors.setLastException(WAKE_UP_EXCEPTION);
            stateMachine.changeStateRejection();
          }

          break;

        case FLIGHT:
          txDataEspNow.setVal(VALVE_OPEN, 0); 
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            Serial.println("Odpalenie error");
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          Serial.println("Komenda odpalenia");
          
          //set options
          stateMachine.changeStateConfirmation();
          break;

        case FIRST_STAGE_RECOVERY:
          //i2c force 1 stage recovery
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceFirstStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          //close main valve
          txDataEspNow.setVal(VALVE_CLOSE, 0); 
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          };

          stateMachine.changeStateConfirmation();
          break;

        case SECOND_STAGE_RECOVERY:
          //i2c force 2 stage recovery
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceSecondStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          //UPUST OPEN
          txDataEspNow.setVal(VALVE_OPEN, 0);
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          stateMachine.changeStateConfirmation();
          break;

        case ON_GROUND:
        /*
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          vTaskDelay(25 / portTICK_PERIOD_MS);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.hardware.i2c1Mutex);
        */
          //UPUST OPEN
          txDataEspNow.setVal(VALVE_OPEN, 0);
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          //MAIN OPEN
          txDataEspNow.setVal(VALVE_OPEN, 0);
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }

          //dataFrame.missionTimer.turnOffTimer();
          
          stateMachine.changeStateConfirmation();
          break;
        
        case HOLD:
          //create disconnect timer
          rc.restartDisconnectTimer(true);

          //disable mission timer
          if(rc.missionTimer.isEnable()){
            rc.missionTimer.turnOffTimer();
          }

          //CLOSE ALL VALVES
          txDataEspNow.setVal(VALVE_CLOSE, 0);
          //Main valve
          if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          //Upust valve
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          //TanWa
          //if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
          //  rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          //}

          stateMachine.changeStateConfirmation();
          break;

        case ABORT:
          rc.deactiveDisconnectTimer();

          if(rc.missionTimer.isEnable()){
            rc.missionTimer.turnOffTimer();
          }

          //Upust valve open
          txDataEspNow.setVal(VALVE_OPEN, 0);
          if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK) rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          

          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          vTaskDelay(25 / portTICK_PERIOD_MS);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          digitalWrite(CAMERA, LOW);

          stateMachine.changeStateConfirmation();
          break;

        default:
          rc.sendLog("Unknown state event");
          ESP.restart();
          break;
      }

      //FIX out of range
      rc.options.sdDataCurrentPeriod = sdPeriod[StateMachine::getCurrentState()];
      rc.options.loraCurrentPeriod = loraPeriod[StateMachine::getCurrentState()];
      rc.options.flashDataCurrentPeriod = flashPeriod[StateMachine::getCurrentState()];
          
      xTaskNotifyGive(rc.hardware.dataTask); //notify dataTask that state change occure to create new lora frame
      stateChangeTimeMark = xTaskGetTickCount() * portTICK_PERIOD_MS;
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    //LOOP 

    if((xTaskGetTickCount() * portTICK_PERIOD_MS - loopTimer) >= STATE_TASK_LOOP_INTERVAL){
      loopTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
    
      switch(StateMachine::getCurrentState()){
        case RDY_TO_LAUNCH:
          if(((xTaskGetTickCount() * portTICK_PERIOD_MS - stateChangeTimeMark) >= 90000) && (rc.dataFrame.pl.isRecording == false)){

            txDataEspNow.setVal(PAYLOAD_RECORCD_ON, 0);
            if(esp_now_send(adressPayLoad, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
          }

          break;
        case COUNTDOWN:
          if(rc.missionTimer.getTime() >= rc.options.ignitionTime && rc.dataFrame.tanWa.igniterContinouity[0] == true){
            txDataEspNow.setVal(IGNITION_COMMAND, 1);  
            //send ignition request
            if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
              rc.sendLog("Esp send error - IGNITION");
            }

            rc.sendLog("IGNITION REQUEST");
          }

          if(rc.missionTimer.getTime() > 0){
            StateMachine::changeStateRequest(States::FLIGHT);
            rc.sendLog("CHANGE TO FLIGHT STATE");
          }

          break;
        case FLIGHT:
          //force main valve open until ocnfirmation
          if(rc.dataFrame.mainValve.valveState != ValveState::Open){
            txDataEspNow.setVal(VALVE_OPEN, 0); 
            Serial.println("Loop odpalenia");
            if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
            //vTaskDelay(25 / portTICK_PERIOD_MS);
          }
          break;
        case FIRST_STAGE_RECOVERY:
          //force recovery until confirmation
          if(rc.dataFrame.recovery.firstStageDone == false){
            xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
            rc.recoveryStm.forceFirstStageSeparation();
            xSemaphoreGive(rc.hardware.i2c1Mutex);
          }
          
          //force main valve close until confirmation
          if(rc.dataFrame.mainValve.valveState != ValveState::Close){
            txDataEspNow.setVal(VALVE_CLOSE, 0); 
            if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
          }

          break;
        
        case SECOND_STAGE_RECOVERY:
          if(rc.dataFrame.recovery.secondStageDone == false){
            xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
            rc.recoveryStm.forceSecondStageSeparation();
            xSemaphoreGive(rc.hardware.i2c1Mutex);
          }

          if(rc.dataFrame.upustValve.valveState != ValveState::Open){
            txDataEspNow.setVal(VALVE_OPEN, 0); 
            Serial.println("Loop odpalenia");
            if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            } 
          }

          break;

        case ON_GROUND:
          if((xTaskGetTickCount() * portTICK_PERIOD_MS - stateChangeTimeMark) >= 30000 && digitalRead(CAMERA) == HIGH){
            digitalWrite(CAMERA, LOW);
          }

          if(rc.dataFrame.mainValve.valveState != ValveState::Open){
            txDataEspNow.setVal(VALVE_OPEN, 0); 
            Serial.println("Loop odpalenia");
            if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
            
          }

          if(rc.dataFrame.upustValve.valveState != ValveState::Open){
            txDataEspNow.setVal(VALVE_OPEN, 0); 
            Serial.println("Loop odpalenia");
            if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
            
          }

          break;

        case ABORT:
          if(rc.dataFrame.recovery.isArmed){
            xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
            rc.recoveryStm.arm(false);
            vTaskDelay(25 / portTICK_PERIOD_MS);
            rc.recoveryStm.setTelemetrum(false);
            xSemaphoreGive(rc.hardware.i2c1Mutex);
          }

          if(rc.dataFrame.upustValve.valveState != ValveState::Open){
            txDataEspNow.setVal(VALVE_OPEN, 0); 
            Serial.println("Loop odpalenia");
            if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
              rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            }
            
          }
        default:
          break;
      }
    }

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}