#include "../include/tasks/tasks.h"
#include "../include/components/runcam.h"


static struct {
  StateMachine stateMachine;
  // TxDataEspNow txDataEspNow;
  TickType_t stateChangeTimeMark;
  TickType_t loopTimer;
}st = {
  .stateMachine = StateMachine(rc.hardware.stateTask),
  .stateChangeTimeMark = 0,
  .loopTimer = 0,
};

static void idle_init(void) {
  rc.options.dataCurrentPeriod = DATA_PERIOD;
  st.stateMachine.changeStateConfirmation();
}

static void armed_init(void) {
  //recovery arm request
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.arm(true);
  vTaskDelay(150);
  rc.recoveryStm.setTelemetrum(true);
  xSemaphoreGive(rc.hardware.i2c1Mutex);
  // TODO:
  //check recovery arm answer 
  st.stateMachine.changeStateConfirmation();
}

static void fueling_init(void) {
  //TODO in future, check valves state
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_CLOSE, 0);
  if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  }
  if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  } 
  st.stateMachine.changeStateConfirmation();
}

static void rdy_to_launch_init(void) {
  RUNCAM_turn_on();
  st.stateMachine.changeStateConfirmation();
}

static void countdown_init(void) {
  if(rc.allDevicesWokenUp() || rc.options.forceLaunch == true){
    //turn on mission timer
    rc.missionTimer.startTimer(millis() + rc.options.countdownTime);
    if(rc.missionTimer.isEnable()){
      if(rc.deactiveDisconnectTimer() == false){
        rc.sendLog("Timer delete error");
      } //turn off disconnectTimer

      st.stateMachine.changeStateConfirmation();
    }else{
      rc.errors.setLastException(MISSION_TIMER_EXCEPTION);
      st.stateMachine.changeStateRejection();
    }
  }else{
    rc.errors.setLastException(WAKE_UP_EXCEPTION);
    st.stateMachine.changeStateRejection();
  }
}

static void flight_init(void) {
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_OPEN, 0); 
  if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    Serial.println("Ignition error");
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  }
  Serial.println("Ignition command");
  //set options
  st.stateMachine.changeStateConfirmation();
}

static void first_stage_recovery_init(void) {
  //i2c force 1 stage recovery
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.forceFirstStageSeparation();
  xSemaphoreGive(rc.hardware.i2c1Mutex);

  //close main valve
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_CLOSE, 0); 
  if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  };

  st.stateMachine.changeStateConfirmation();
}

static void second_stage_recovery_init(void) {
  //i2c force 2 stage recovery
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.forceSecondStageSeparation();
  xSemaphoreGive(rc.hardware.i2c1Mutex);

  //UPUST OPEN
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_OPEN, 0);
  if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  }
          
  st.stateMachine.changeStateConfirmation();
}

static void on_ground_init(void) {
  /*
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.arm(false);
  vTaskDelay(25 / portTICK_PERIOD_MS);
  rc.recoveryStm.setTelemetrum(false);
  xSemaphoreGive(rc.hardware.i2c1Mutex);
  */
  //UPUST OPEN
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_OPEN, 0);
  if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  }

  //MAIN OPEN
  txDataEspNow.setVal(VALVE_OPEN, 0);
  if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
  }
  st.stateMachine.changeStateConfirmation();
}

static void hold_init(void) {
  //create disconnect timer
  rc.restartDisconnectTimer(true);

  //disable mission timer
  if(rc.missionTimer.isEnable()){
  rc.missionTimer.turnOffTimer();
  }

  //CLOSE ALL VALVES
  TxDataEspNow txDataEspNow;
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
  RUNCAM_turn_off();
  st.stateMachine.changeStateConfirmation();
}


static void abort_init(void) {
  rc.deactiveDisconnectTimer();

  if(rc.missionTimer.isEnable()){
    rc.missionTimer.turnOffTimer();
  }

  //Upust valve open
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(VALVE_OPEN, 0);
  if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK) rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          

  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.arm(false);
  vTaskDelay(25 / portTICK_PERIOD_MS);
  rc.recoveryStm.setTelemetrum(false);
  xSemaphoreGive(rc.hardware.i2c1Mutex);

  RUNCAM_turn_off();

  st.stateMachine.changeStateConfirmation();
}

static void countdown_loop(void) {
  if(rc.missionTimer.getTime() > 0){
    StateMachine::changeStateRequest(States::FLIGHT);
    rc.sendLog("CHANGE TO FLIGHT STATE");
  }
}

static void flight_loop(void) {
  //force main valve open until ocnfirmation
  if(rc.dataFrame.mainValve.valveState != ValveState::Open){
    TxDataEspNow txDataEspNow;
    txDataEspNow.setVal(VALVE_OPEN, 0); 
    Serial.println("Main valve open loop");
    if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    }
    //vTaskDelay(25 / portTICK_PERIOD_MS);
  }
}

static void first_stage_recovery_loop(void) {
  //force recovery until confirmation
  if(rc.dataFrame.recovery.firstStageDone == false){
    xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
    rc.recoveryStm.forceFirstStageSeparation();
    xSemaphoreGive(rc.hardware.i2c1Mutex);
  }
          
  //force main valve close until confirmation
  if(rc.dataFrame.mainValve.valveState != ValveState::Close){
    TxDataEspNow txDataEspNow;
    txDataEspNow.setVal(VALVE_CLOSE, 0); 
    if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    }
  }
}

static void second_stage_recovery_loop(void) {
  if(rc.dataFrame.recovery.secondStageDone == false){
    xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
    rc.recoveryStm.forceSecondStageSeparation();
    xSemaphoreGive(rc.hardware.i2c1Mutex);
  }

  if(rc.dataFrame.upustValve.valveState != ValveState::Open){
    TxDataEspNow txDataEspNow;
    txDataEspNow.setVal(VALVE_OPEN, 0); 
    Serial.println("Loop odpalenia");
    if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    } 
  }
}

static void on_ground_loop(void) {
  TxDataEspNow txDataEspNow;
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
}

static void abort_loop(void) {
  if(rc.dataFrame.recovery.isArmed){
    xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
    rc.recoveryStm.arm(false);
    vTaskDelay(25 / portTICK_PERIOD_MS);
    rc.recoveryStm.setTelemetrum(false);
    xSemaphoreGive(rc.hardware.i2c1Mutex);
  }

  if(rc.dataFrame.upustValve.valveState != ValveState::Open){
    TxDataEspNow txDataEspNow;
    txDataEspNow.setVal(VALVE_OPEN, 0); 
    Serial.println("Loop odpalenia");
    if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    }
    
  }
}

static void state_init(void){
  //portENTER_CRITICAL(&rc.stateLock);
  switch(st.stateMachine.getRequestedState()){
    case IDLE:
      idle_init();
      break;
    case ARMED:
      armed_init();
      break;
    case FUELING:
      fueling_init();
      break;
    case RDY_TO_LAUNCH:
      rdy_to_launch_init();
      break;
    case COUNTDOWN:
      countdown_init();
      break;
    case FLIGHT:
      flight_init();
      break;
    case FIRST_STAGE_RECOVERY:
      first_stage_recovery_init();
      break;
    case SECOND_STAGE_RECOVERY:
      second_stage_recovery_init();
      break;
    case ON_GROUND:
      on_ground_init();
      break;  
    case HOLD:
      hold_init();
      break;
    case ABORT:
      abort_init();
      break;
    
    default:
      rc.sendLog("Unknown state event");
      ESP.restart();
      break;
  }
  //Set new periods based on new state
  //TODO: FIX out of range
  rc.options.sdDataCurrentPeriod = sdPeriod[StateMachine::getCurrentState()];
  rc.options.loraCurrentPeriod = loraPeriod[StateMachine::getCurrentState()];
  rc.options.flashDataCurrentPeriod = flashPeriod[StateMachine::getCurrentState()];
          
  xTaskNotifyGive(rc.hardware.dataTask); //notify dataTask that state change occure to create new lora frame
  st.stateChangeTimeMark = xTaskGetTickCount() * portTICK_PERIOD_MS;
  //portEXIT_CRITICAL(&rc.stateLock);
}

static void state_loop(void) {
  TxDataEspNow txDataEspNow;
  switch(StateMachine::getCurrentState()){
    case RDY_TO_LAUNCH:
      // move to timer
      if(((xTaskGetTickCount() * portTICK_PERIOD_MS - st.stateChangeTimeMark) >= 90000) && (rc.dataFrame.pl.isRecording == false)){
        txDataEspNow.setVal(PAYLOAD_RECORCD_ON, 0);
        if(esp_now_send(adressPayLoad, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
          rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
        }
      }
      break;
    case COUNTDOWN:
      // move to timer
      if(rc.missionTimer.getTime() >= rc.options.ignitionTime && rc.dataFrame.tanWa.igniterContinouity[0] == true){
        txDataEspNow.setVal(IGNITION_COMMAND, 1);  
        //send ignition request
        if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
          rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
          rc.sendLog("Esp send error - IGNITION");
        }
        rc.sendLog("IGNITION REQUEST");
      }
      countdown_loop();
      break;
    case FLIGHT:
      flight_loop();
      break;
    case FIRST_STAGE_RECOVERY:
      first_stage_recovery_loop();
      break;
    case SECOND_STAGE_RECOVERY:
      second_stage_recovery_loop();
      break;
    case ON_GROUND:
      // move to timer
      if((xTaskGetTickCount() * portTICK_PERIOD_MS - st.stateChangeTimeMark) >= 30000 && digitalRead(CAMERA) == HIGH){
        RUNCAM_turn_off();
      }
      on_ground_loop();
      break;
    case ABORT:
      abort_loop();
     break;
    default:
      break;
  }
}

void stateTask(void *arg){
  TxDataEspNow txDataEspNow;
  
  while(1){
    if(ulTaskNotifyTake(pdTRUE, 0)){
      state_init();
    }

    //LOOP 
    if((xTaskGetTickCount() * portTICK_PERIOD_MS - st.loopTimer) >= STATE_TASK_LOOP_INTERVAL){
      st.loopTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      state_loop();
    }

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}