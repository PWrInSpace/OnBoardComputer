#include "../include/tasks/tasks.h"

void stateTask(void *arg){
  TxDataEspNow txDataEspNow;
  char log[LORA_FRAME_ARRAY_SIZE] = {};
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 0)){
      //portENTER_CRITICAL(&rc.stateLock);
      //setup
      
      switch(rc.stateEvent){
        case IDLE_EVENT:
          rc.changeState(IDLE);
          break;

        case ARMED_EVENT:
          //recovery arm request
          xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(true);
          rc.recoveryStm.setTelemetrum(true);
          xSemaphoreGive(rc.i2c1Mutex);
          //check recovery arm answer
          
          rc.changeState(ARMED);
          break;

        case FUELING_EVENT:
          //TODO in future, check valves state
          rc.options.sdDataCurrentPeriod = rc.options.sdLongPeriod * portTICK_PERIOD_MS;

          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
            
          rc.changeState(FUELING);
          break;

        case RDY_TO_LAUNCH_EVENT:
          digitalWrite(CAMERA, HIGH); //turn on camera
          rc.changeState(RDY_TO_LAUNCH);
          break;

        case COUNTDOWN_EVENT:
          //dataframe 
          if(dataFrame.allDevicesWokeUp() || rc.options.forceLaunch == true){
            
            //set options
            rc.options.sdDataCurrentPeriod = rc.options.sdShortPeriod * portTICK_PERIOD_MS;
            rc.options.flashDataCurrentPeriod = rc.options.flashShortPeriod * portTICK_PERIOD_MS;

            //turn on mission timer
            dataFrame.missionTimer.startTimer(millis() + rc.options.countdownTime);

            if(dataFrame.missionTimer.isEnable()){
              if(xTimerDelete(rc.disconnectTimer, 25) == pdFALSE){
                strcpy(log, "Timer delete error");
                rc.sendLog(log);
              } //turn off disconnectTimer
              rc.disconnectTimer = NULL;
              
              rc.changeState(COUNTDOWN);
            }else{
              dataFrame.errors.setLastException(MISSION_TIMER_EXCEPTION);
              rc.unsuccessfulEvent();
            }
          }else{
            dataFrame.errors.setLastException(WAKE_UP_EXCEPTION);
            rc.unsuccessfulEvent();
          }

          break;

        case FLIGHT_EVENT:
          txDataEspNow.setVal(VALVE_OPEN, 0); //IDK
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK
          
          //set options
         

          rc.changeState(FLIGHT);
          break;

        case FIRST_STAGE_RECOVERY_EVENT:
          //i2c force 1 stage recovery
          xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceFirstStageSeparation();
          xSemaphoreGive(rc.i2c1Mutex);

          txDataEspNow.setVal(VALVE_CLOSE, 0);
          esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));

          rc.options.flashDataCurrentPeriod = rc.options.flashLongPeriod * portTICK_PERIOD_MS;
          
          rc.changeState(FIRST_STAGE_RECOVERY);
          break;

        case SECOND_STAGE_RECOVERY_EVENT:
          //i2c force 2 stage recovery
          xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.forceSecondStageSeparation();
          xSemaphoreGive(rc.i2c1Mutex);

          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          
          rc.changeState(SECOND_STAGE_RECOVERY);
          break;

        case ON_GROUND_EVENT:
          rc.options.sdDataCurrentPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.loraPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;

          xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.i2c1Mutex);

          //dataFrame.missionTimer.turnOffTimer();
          
          rc.changeState(ON_GROUND);
          break;

        case ABORT_EVENT:
          if(rc.disconnectTimer != NULL){
            xTimerDelete(rc.disconnectTimer, 25); //turn off disconnectTimer
          }
      
          txDataEspNow.setVal(VALVE_OPEN, 0);
          esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)); //IDK

          xSemaphoreTake(rc.i2c1Mutex, portMAX_DELAY);
          rc.recoveryStm.arm(false);
          rc.recoveryStm.setTelemetrum(false);
          xSemaphoreGive(rc.i2c1Mutex);

          rc.options.sdDataCurrentPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.dataFramePeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;
          rc.options.loraPeriod = rc.options.idlePeriod * portTICK_PERIOD_MS;

          rc.changeState(ABORT);
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
    switch(rc.state){
      case COUNTDOWN:
        if(dataFrame.missionTimer.getTime() >= rc.options.ignitionTime && dataFrame.ignition == false){
          txDataEspNow.setVal(IGNITION_COMMAND, 0);  //IDK
          //send ignition request
          esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow));
          dataFrame.ignition = true;
        }

        if(dataFrame.missionTimer.getTime() > 0){
          rc.changeStateEvent(FLIGHT_EVENT);
        }

        break;
      default:
        break;
    }
    

    //Serial.println("State TASK"); //DEBUG

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}