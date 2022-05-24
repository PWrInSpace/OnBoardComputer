#include "../include/tasks/tasks.h"

void stateTask(void *arg){
  StateMachine stateMachine(rc.hardware.stateTask);
  TxDataEspNow txDataEspNow;
  
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
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          }
          
          //set options
          stateMachine.changeStateConfirmation();
          break;

        case FIRST_STAGE_RECOVERY:
          //i2c force 1 stage recovery
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceFirstStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          //close main valve
          //txDataEspNow.setVal(VALVE_CLOSE, 0); 
          //if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
          //  rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          //};

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
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          vTaskDelay(25 / portTICK_PERIOD_MS);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.hardware.i2c1Mutex);

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
      
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    //LOOP 
    switch(StateMachine::getCurrentState()){
      case COUNTDOWN:
        if(rc.missionTimer.getTime() >= rc.options.ignitionTime && rc.dataFrame.tanWa.igniterContinouity[0] == true){
          txDataEspNow.setVal(IGNITION_COMMAND, 0);  //IDK
          //send ignition request
          if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
            rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
            rc.sendLog("Esp send error - IGNITION");
          }

          //TODO uncomment
          //rc.sendLog("IGNITION REQUEST");
        }

        if(rc.missionTimer.getTime() > 0){
          StateMachine::changeStateRequest(States::FLIGHT);
          rc.sendLog("CHANGE TO FLIGHT STATE");
        }

        break;
      case FIRST_STAGE_RECOVERY:
        if(rc.dataFrame.recovery.firstStageDone == false){
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceFirstStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);
        }
        break;
      
      case SECOND_STAGE_RECOVERY:
        if(rc.dataFrame.recovery.secondStageDone == false){
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceSecondStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);
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
      default:
        break;
    }

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}