#include "../include/tasks/tasks.h"

void stateTask(void *arg){
  StateMachine stateMachine(rc.hardware.stateTask);
  TxDataEspNow txDataEspNow;
  char log[LORA_FRAME_ARRAY_SIZE] = {};
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 0)){
      //portENTER_CRITICAL(&rc.stateLock);
      //setup
      
      switch(stateMachine.getRequestedState()){
        case IDLE:
          stateMachine.changeStateConfirmation();
          break;

        case ARMED:
          //recovery arm request
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(true);
          rc.recoveryStm.setTelemetrum(true);
          xSemaphoreGive(rc.hardware.i2c1Mutex);
          //check recovery arm answer
          
          stateMachine.changeStateConfirmation();
          break;

        case FUELING:
          //TODO in future, check valves state
          rc.options.sdDataCurrentPeriod = rc.options.sdLongPeriod * portTICK_PERIOD_MS;

          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
            
          stateMachine.changeStateConfirmation();
          break;

        case RDY_TO_LAUNCH:
          digitalWrite(CAMERA, HIGH); //turn on camera
          stateMachine.changeStateConfirmation();
          break;

        case COUNTDOWN:
          //dataframe 
          if(rc.allDevicesWokenUp() || rc.options.forceLaunch == true){
            
            //set options
            rc.options.sdDataCurrentPeriod = rc.options.sdShortPeriod * portTICK_PERIOD_MS;
            rc.options.flashDataCurrentPeriod = rc.options.flashShortPeriod * portTICK_PERIOD_MS;

            //turn on mission timer
            rc.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(rc.missionTimer.isEnable()){
              if(xTimerDelete(rc.hardware.disconnectTimer, 25) == pdFALSE){
                strcpy(log, "Timer delete error");
                rc.sendLog(log);
              } //turn off disconnectTimer
              rc.hardware.disconnectTimer = NULL;
              
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
          txDataEspNow.setVal(VALVE_OPEN, 0); //IDK
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          //set options
          stateMachine.changeStateConfirmation();
          break;

        case FIRST_STAGE_RECOVERY:
          //i2c force 1 stage recovery
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceFirstStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));

          rc.options.flashDataCurrentPeriod = rc.options.flashLongPeriod * portTICK_PERIOD_MS;
          
          stateMachine.changeStateConfirmation();
          break;

        case SECOND_STAGE_RECOVERY:
          //i2c force 2 stage recovery
          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceSecondStageSeparation();
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          stateMachine.changeStateConfirmation();
          break;

        case ON_GROUND:
          rc.options.sdDataCurrentPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.loraPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;

          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          //dataFrame.missionTimer.turnOffTimer();
          
          stateMachine.changeStateConfirmation();
          break;

        case ABORT:
          if(rc.hardware.disconnectTimer != NULL){
            xTimerDelete(rc.hardware.disconnectTimer, 25); //turn off disconnectTimer
          }
      
          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.hardware.i2c1Mutex);

          rc.options.sdDataCurrentPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.loraPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;

          stateMachine.changeStateConfirmation();
          break;

        default:
          strcpy(log, "Unknown state event");
          rc.sendLog(log);
          ESP.restart();
          break;
      }
      //portEXIT_CRITICAL(&rc.stateLock);
    }

    //Serial.println("Test");
    switch(StateMachine::getCurrentState()){
      case COUNTDOWN:
        if(rc.missionTimer.getTime() >= rc.options.ignitionTime && rc.dataFrame.mcb.ignition == false){
          txDataEspNow.setVal(IGNITION_COMMAND, 0);  //IDK
          //send ignition request
          esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          rc.dataFrame.mcb.ignition = true;
        }

        if(rc.missionTimer.getTime() > 0){
          StateMachine::changeStateRequest(States::FLIGHT);
        }

        break;
      default:
        break;
    }

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}