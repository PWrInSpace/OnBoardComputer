#include "../include/tasks/tasks.h"
#include "../include/components/runcam.h"

static void payload_swtich_on_cb(void *arg) {
  TxDataEspNow txDataEspNow;
  txDataEspNow.setVal(PAYLOAD_RECORCD_ON, 0);
  Serial.println("Payload recording on command");
  if(esp_now_send(adressPayLoad, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
    rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    Serial.println("Payload recording send error");
  }
}

static void turn_off_recording_cb(void *arg) {
  Serial.println("Turn camera off");
  RUNCAM_turn_off();
}

static void ignition_cb(void *arg) {
  TxDataEspNow txDataEspNow;
  Serial.println("***Ignition cb***");
  // if(rc.missionTimer.getTime() >= rc.options.ignitionTime && rc.dataFrame.tanWa.igniterContinouity[0] == true){
    txDataEspNow.setVal(IGNITION_COMMAND, 1);  
        //send ignition request
    if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
      rc.sendLog("Esp send error - IGNITION");
    }
    rc.sendLog("IGNITION REQUEST");
  // }
}

static struct {
  TickType_t stateChangeTimeMark;
  TickType_t loopTimer;
  TimerHandle_t payload_switch_on_rpi;
  TimerHandle_t turn_off_recording;
  TimerHandle_t ignition_request;
}st = {
  .stateChangeTimeMark = 0,
  .loopTimer = 0,
  .payload_switch_on_rpi = xTimerCreate("Payload", 
    PAYLOAD_SWITCH_ON_AFTER_STATE_TIME,
    pdFALSE,
    TIMER_PAYLOAD_SWITCH_ON_NUM,
    payload_swtich_on_cb),
  .turn_off_recording = xTimerCreate("Camera off",
    CAMERA_TURN_OFF_TIME,
    pdFALSE,
    TIMER_TURN_OFF_RECORDING_NUM,
    turn_off_recording_cb),
  .ignition_request = xTimerCreate("Ignition", 
    (OPT_get_iginition_time() - OPT_get_countdown_begin_time()) / portTICK_PERIOD_MS,
    pdFALSE,
    TIMER_IGNITION_REQUEST_NUM,
    ignition_cb),
};

static void idle_init(void) {
  OPT_set_data_current_period(DATA_PERIOD);
  SM_changeStateConfirmation();
}

static void recovery_arm_init(void) {
    //recovery arm request
  xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
  rc.recoveryStm.arm(true);
  vTaskDelay(150);
  rc.recoveryStm.setTelemetrum(true);
  xSemaphoreGive(rc.hardware.i2c1Mutex);
  // TODO:
  //check recovery arm answer 
  SM_changeStateConfirmation();
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
  SM_changeStateConfirmation();
}

static void armed_to_launch_init(void) {
  //wake up
  // TODO:
}

static void rdy_to_launch_init(void) {
  RUNCAM_turn_on();
  xTimerStart(st.payload_switch_on_rpi, portMAX_DELAY); //TODO: block
  SM_changeStateConfirmation();
}

static void countdown_init(void) {
  if(rc.allDevicesWokenUp() || OPT_get_force_launch() == true){
    //turn on mission timer
    rc.missionTimer.startTimer(OPT_get_countdown_begin_time());
    if(rc.missionTimer.isEnable()){
      if(rc.deactiveDisconnectTimer() == false){
        rc.sendLog("Timer delete error");
      } //turn off disconnectTimer
      
      uint32_t time_to_ignition = OPT_get_iginition_time() - OPT_get_countdown_begin_time();
      assert(time_to_ignition > 0);
      xTimerChangePeriod(st.ignition_request, time_to_ignition/portTICK_PERIOD_MS, portMAX_DELAY);
      xTimerStart(st.ignition_request, portMAX_DELAY);

      SM_changeStateConfirmation();
    }else{
      rc.errors.setLastException(MISSION_TIMER_EXCEPTION);
      SM_changeStateRejection();
    }
  }else{
    rc.errors.setLastException(WAKE_UP_EXCEPTION);
    SM_changeStateRejection();
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
  SM_changeStateConfirmation();
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

  SM_changeStateConfirmation();
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
          
  SM_changeStateConfirmation();
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
  SM_changeStateConfirmation();

  xTimerStart(st.turn_off_recording, portMAX_DELAY);
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
  SM_changeStateConfirmation();
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

  SM_changeStateConfirmation();
}

static void countdown_loop(void) {
  if(rc.missionTimer.getTime() > 0){
    SM_changeStateRequest(States::FLIGHT);
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
    if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      rc.errors.setEspNowError(ESPNOW_SEND_ERROR);
    }
  }
}

static void state_init(void){
  //portENTER_CRITICAL(&rc.stateLock);
  switch(SM_getRequestedState()){
    case IDLE:
      idle_init();
      break;
    case RECOVERY_ARM:
      recovery_arm_init();
      break;
    case FUELING:
      fueling_init();
      break;
    case ARMED_TO_LAUNCH:
      armed_to_launch_init();
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
  OPT_set_data_current_period(sdPeriod[SM_getCurrentState()]);
  OPT_set_lora_current_period(loraPeriod[SM_getCurrentState()]);
  OPT_set_flash_write_current_period(flashPeriod[SM_getCurrentState()]);
          
  xTaskNotifyGive(rc.hardware.dataTask); //notify dataTask that state change occure to create new lora frame
  st.stateChangeTimeMark = xTaskGetTickCount() * portTICK_PERIOD_MS;
  //portEXIT_CRITICAL(&rc.stateLock);
}

static void state_loop(void) {
  TxDataEspNow txDataEspNow;
  switch(SM_getCurrentState()){
    case COUNTDOWN:      
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

    if((xTaskGetTickCount() * portTICK_PERIOD_MS - st.loopTimer) >= STATE_TASK_LOOP_INTERVAL){
      st.loopTimer = xTaskGetTickCount() * portTICK_PERIOD_MS;
      state_loop();
    }

    wt.stateTaskFlag = true;
    vTaskDelay(10 / portTICK_PERIOD_MS); //DEBUG TIME
  }
}