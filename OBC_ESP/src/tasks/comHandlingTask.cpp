#include "../include/tasks/tasks.h"
#include "options.h"

static void R4A_change_state(char * data) {
  assert(data != NULL);
  int old_state;
  int new_state;
  sscanf(data, "R4A;STAT;%d;%d", &old_state, &new_state);

  if (old_state > States::ABORT || new_state > States::ABORT) {
    rc.sendLog("invalid state change command");
    ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    return;
  }

  if (old_state != SM_getCurrentState() || new_state != States::FLIGHT) {
    rc.sendLog("invalid state change command");
    ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    return;
  }

  if(SM_changeStateRequest((States) new_state) == false){
      rc.sendLog("invalid state change request");
      ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
  }
}

static void R4A_abort(void) {
    if(SM_changeStateRequest(States::ABORT) == false){
        rc.sendLog("invalid ABORT request");
        ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    }
}

static void R4A_hold_in(void) {
    if(SM_getCurrentState() == States::HOLD) {
        rc.sendLog("invalid HOLD_IN request");
        ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
        return;
    }

    if(SM_changeStateRequest(States::HOLD) == false){
        rc.sendLog("invalid HOLD_IN request");
        ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    }
}

static void R4A_hold_out(void) {
    if(SM_getCurrentState() != States::HOLD) {
        rc.sendLog("invalid HOLD_OUT request");
        ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
        return;
    }

    if(SM_changeStateRequest(States::HOLD) == false){
        rc.sendLog("invalid HOLD_OUT request");
        ERR_set_last_exception(INVALID_STATE_CHANGE_EXCEPTION);
    }
}

static void R4A_invalid_command(char *data) {
    strcat(data, " - INVALID COMMAND");
    rc.sendLog(data);
    ERR_set_last_exception(INVALID_PREFIX_EXCEPTION);
}

static void handle_R4A_message(char *data) {
    if (strstr(data, "STAT;") != NULL) {
        R4A_change_state(data);
    }else if (strstr(data, "ABRT") != NULL) {
        R4A_abort();
    }else if (strstr(data, "HOLD_IN") != NULL){
        R4A_hold_in();
    }else if (strstr(data, "HOLD_OUT") != NULL){
        R4A_hold_out();
    }else{
        R4A_invalid_command(data);
    }
}

static void R4O_options_lora_freq(int value) {
    if (OPT_set_lora_freq(value) == false) {
        rc.sendLog("Invalid lora frequency");
        ERR_set_last_exception(INVALID_OPTION_VALUE);
        return;
    }

    xSemaphoreTake(rc.hardware.spiMutex, portMAX_DELAY);
    LoRa.setFrequency(OPT_get_lora_freq() * 1E3);
    xSemaphoreGive(rc.hardware.spiMutex);
}

static void R4O_options_countdown_begin_time(int value) {
    if (OPT_set_countdown_begin_time(value) == false) {
      rc.sendLog("Invalid countdown time set");
      ERR_set_last_exception(INVALID_OPTION_VALUE);
    }

    rc.missionTimer.setDisableValue(OPT_get_countdown_begin_time());
}

static void R4O_options_ignition_time(int value) {
    if (OPT_set_ignition_time(value) == false) {
        rc.sendLog("Invalid ignition time set");
        ERR_set_last_exception(INVALID_OPTION_VALUE);
    }
}

static void R4O_options_tank_min_pressure(int value) {
    if (OPT_set_tank_min_pressure(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_flash_write(int value) {
    if (OPT_set_flash_write(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_(int value) {
    if (OPT_set_force_launch(value) == false) {
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_option_data_period(int value) {
    if (OPT_set_data_current_period(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_lora_period(int value) {
     if(OPT_set_lora_current_period(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_flash_period(int value) {
     if(OPT_set_flash_write_current_period(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_sd_period(int value) {
     if(OPT_set_sd_write_current_period(value) == false){
        ERR_set_last_exception(INVALID_OPTION_NUMBER);
    }
}

static void R4O_options_invalid(void) {
    rc.sendLog("Invalid option NUMBER");
    ERR_set_last_exception(INVALID_OPTION_NUMBER);
}

static void R4O_handle_options(char * data){
    assert(data != NULL);
    int option_number;
    int option_value;
    sscanf(data, "R4O;OPTS;%d;%d", &option_number, &option_value);

     switch (option_number) {
          case 1:
            R4O_options_lora_freq(option_value);
            break;
          case 2:
            R4O_options_countdown_begin_time(option_value);
            break;
          case 3:
            R4O_options_ignition_time(option_value);
            break;
          case 4:
            R4O_options_tank_min_pressure(option_value);
            break;
          case 5:
            R4O_options_flash_write(option_value);
            break;
          case 6:
            break;
          case 7:
            R4O_option_data_period(option_value);
            break;
          case 8:
           R4O_options_lora_period(option_value);
            break;
          case 9:
            R4O_options_flash_period(option_value);
            break;
          case 10:
            R4O_options_sd_period(option_value);
            break;
          default:
            R4O_options_invalid();
          }
}

static void R4O_handle_main_valve(char *data) {
    assert(data != NULL);
    TxDataEspNow txDataEspNow;
    sscanf(data, "R4O;MVAL;%d;%d", &txDataEspNow.command, &txDataEspNow.commandTime);
    if(esp_now_send(adressMValve, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void R4O_handel_upust_valve(char *data) {
    TxDataEspNow txDataEspNow;
    sscanf(data, "R4O;UVAL;%d;%d", &txDataEspNow.command, &txDataEspNow.commandTime);
    if(esp_now_send(adressUpust, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void R4O_handle_wake_up(void) {
    States tempState = SM_getCurrentState();
    while(tempState < States::COUNTDOWN){
        valvePeriod[tempState] = 500;
        pitotPeriod[tempState] = 500;
        espNowDefaultPeriod[tempState] = 500;
        tempState = static_cast<States>(tempState + 1);
    }
}

static void R4O_handle_go_off(void) {
    States tempState = SM_getCurrentState();
    valvePeriod[tempState] = ESP_NOW_SLEEP_TIME;
    pitotPeriod[tempState] = ESP_NOW_SLEEP_TIME;
    espNowDefaultPeriod[tempState] = ESP_NOW_SLEEP_TIME;
}

static void R4O_handle_recovery(char *data) {
    TxDataEspNow txDataEspNow;
    sscanf(data, "R4O;RECOV;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
    xSemaphoreTake(rc.hardware.i2c1Mutex, portMAX_DELAY);
    rc.recoveryStm.sendCommand(txDataEspNow.command, txDataEspNow.commandTime);
    xSemaphoreGive(rc.hardware.i2c1Mutex);
}

static void R4O_handle_tanwa(char *data) {
    TxDataEspNow txDataEspNow;
    sscanf(data, "R4O;TANWA;%d;%d", (int*) &txDataEspNow.command, (int*) &txDataEspNow.commandTime);
    if(esp_now_send(adressTanWa, (uint8_t*) &txDataEspNow, sizeof(txDataEspNow)) != ESP_OK){
      ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void R4O_handle_soft_reset(void) {
    ESP.restart();
}

static void R4O_handle_void_command(void) {
    rc.sendLog("VOID command appear");
}

static void R4O_handle_invalid(char *data) {
    strcat(data, " - INVALID COMMAND");
    rc.sendLog(data);
    ERR_set_last_exception(INVALID_PREFIX_EXCEPTION);
}

static void R4O_create_and_send_options_frame(void) {
    char buffer[LORA_FRAME_ARRAY_SIZE];
    rc.createOptionsFrame(buffer, OPT_get_options_struct());
    xQueueSend(rc.hardware.loraTxQueue, (void*)buffer, 0);
}

static void handle_R4O_message(char *data){
    assert(data != NULL);
    if (strstr(data, "OPTS;") != NULL) {
        R4O_handle_options(data);
        R4O_create_and_send_options_frame();
    } else if (strstr(data, "MVAL;") != NULL) {
        R4O_handle_main_valve(data);
    }else if (strstr(data, "UVAL;") != NULL) {
        R4O_handel_upust_valve(data);
    }else if (strstr(data, "WKUP") != NULL) {
        R4O_handle_wake_up();
    }else if (strstr(data, "GOFF") != NULL){
        R4O_handle_go_off();
    }else if (strstr(data, "RECOV") != NULL) {
        R4O_handle_recovery(data);
    }else if(strstr(data, "TANWA") != NULL){
        R4O_handle_tanwa(data);
    }else if (strstr(data, "RST")){
        R4O_handle_soft_reset();
    } else if (strstr(data, "VOID")){
        R4O_handle_void_command();
    }else{
        R4O_handle_invalid(data);
    }
}

static void handle_invalid_lora_message(char *data) {
    strcat(data, " - INVALID PREFIX");
    rc.sendLog(data);
    ERR_set_last_exception(INVALID_PREFIX_EXCEPTION);
}

static void esp_now_check_correctness(void) {
    // uint16_t sleepTime;
    // uint8_t currentState = SM_getCurrentState();

    // if(currentState >= PERIOD_ARRAY_SIZE){
    //     rc.sendLog("Out of period array size :C");
    //     rxEspNumber = 0xff;
    // }

    // if(rxEspNumber < CONNECTION_CHECK_DEVICE_NUMBER){
    //     //Serial.print("Ustawiam: ");
    //     //Serial.println(rxEspNumber);
    //     rc.isConnectedFlags[rxEspNumber] = true;
    // }
}

static void esp_now_tanwa(void) {
    Serial.println("TanWa notify"); //DEBUG
}

static void esp_now_pitot(void) {
    Serial.println("Pitot notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = pitotPeriod[currentState]; //TODO: state options

    if(esp_now_send(adressPitot, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
        rc.isConnectedFlags[PITOT] = false;
    }
}

static void esp_now_main_valve(void) {
    Serial.println("Main valve notify");
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = valvePeriod[currentState];

    if(esp_now_send(adressMValve, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
      ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
      rc.isConnectedFlags[MAIN_VALVE] = false;
    }
}

static void esp_now_upust_valve(void) {
    Serial.println("UpustValve notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = valvePeriod[currentState];
    if(esp_now_send(adressUpust, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_black_box(void) {
    Serial.println("Black Box notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = espNowDefaultPeriod[currentState];

    if(esp_now_send(adressBlackBox, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_payload(void) {
    Serial.println("Payload notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = payloadPeriod[currentState];

    if(esp_now_send(adressPayLoad, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_acs(void) {
    Serial.println("ACS notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = espNowDefaultPeriod[currentState];

    if(esp_now_send(adressACS, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_camera_acs(void) {
    Serial.println("camera ACS notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = espNowDefaultPeriod[currentState];

    if(esp_now_send(cameraACS, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
      ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_camera_pitot(void) {
    Serial.println("Camera pitot notify"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = espNowDefaultPeriod[currentState];

    if(esp_now_send(cameraPitot, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
        ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_camera_recovery(void) {
    Serial.println("Camera recovery"); //DEBUG
    States currentState = SM_getCurrentState();
    uint16_t sleepTime = espNowDefaultPeriod[currentState];

    if(esp_now_send(cameraRecovery, (uint8_t*) &sleepTime, sizeof(sleepTime)) != ESP_OK){
      ERR_set_esp_now_error(ESPNOW_SEND_ERROR);
    }
}

static void esp_now_handle(EspNowDevice esp_device) {
    switch(esp_device){
        case TANWA:
            esp_now_tanwa();
            break;
        case PITOT:
            esp_now_pitot();
            break;
        case MAIN_VALVE:
            esp_now_main_valve();
            break;
        case UPUST_VALVE:
            esp_now_upust_valve();
          break;
        case BLACK_BOX:
            esp_now_black_box();
            break;
        case PAYLOAD:
            esp_now_payload();
            break;
        case ACS:
            esp_now_acs();
            break;
        case CAMERA_ACS:
            esp_now_camera_acs();
            break;
        case CAMERA_PITOT:
            esp_now_camera_pitot();
            break;
        case CAMERA_RECOVERY:
            esp_now_camera_recovery();
            break;
        default:
            rc.sendLog("Unknown esp now device");
            break;
      }
}

void rxHandlingTask(void *arg){
    char buffer[LORA_FRAME_ARRAY_SIZE];
    EspNowDevice esp_device;
    memset(buffer, 0 ,sizeof(buffer));

    while(1){
        if (xQueueReceive(rc.hardware.loraRxQueue, (void*)&buffer, 25) == pdTRUE) {
            if(strncmp(buffer, "R4A", 3) == 0){
                rc.restartDisconnectTimer();
                handle_R4A_message(buffer);
            }else if(strncmp(buffer, "R4O", 3) == 0){
                rc.restartDisconnectTimer();
                handle_R4O_message(buffer);
            }else{
                handle_invalid_lora_message(buffer);
            }
        }

        if (xQueueReceive(rc.hardware.espNowQueue, (void*) &esp_device, 25)) {
            esp_now_handle(esp_device);
        }
    }

    wt.rxHandlingTaskFlag = true;
    vTaskDelay(50 / portTICK_PERIOD_MS);
}
