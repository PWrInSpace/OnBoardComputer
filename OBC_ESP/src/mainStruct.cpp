#include "mainStructs.h"

/******   OPTIONS   *******/
Options::Options():
  LoRaFrequencyMHz(LoRa_FREQUENCY_MHZ),
  countdownTime(COUNTDOWN_TIME),
  ignitionTime(IGNITION_TIME),
  tankMinPressure(TANK_MIN_PRESSURE),
  flashWrite(FLASH_WRITE),
  forceLaunch(FORCE_LAUNCH),
  mainValveRequestState(MAIN_VALVE_REQUEST_STATE),
  upustValveRequestState(UPUST_VALVE_REQUEST_STATE),
  mainValveCommandTime(MAIN_VALVE_COMMAND_TIME),
  upustValveCommandTime(UPUST_VALVE_COMMAND_TIME),
  espnowSleepTime(ESP_NOW_SLEEP_TIME),
  espnowFastPeriod(ESP_NOW_FAST_PERIOD),
  espnowSlowPeriod(ESP_NOW_SLOW_PERIOD),
  loraFastPeriod(LORA_FAST_PERIOD),
  loraSlowPeriod(LORA_SLOW_PERIOD),
  dataFastPeriod(DATA_FAST_PERIOD),
  flashFastPeriod(FLASH_FAST_PERIOD),
  flashSlowPeriod(FLASH_SLOW_PERIOD),
  sdFastPeriod(SD_FAST_PERIOD),
  sdSlowPeriod(SD_SLOW_PERIOD),
  sharedPeriod(SHARED_PERIOD),
  dataFramePeriod(DATA_FAST_PERIOD),
  loraDataPeriod(LORA_SLOW_PERIOD),
  flashDataPeriod(FLASH_SLOW_PERIOD),
  sdDataPeriod(SD_SLOW_PERIOD)
{}

/****** MAINSTRUCT *******/

RocketControl::RocketControl():
  stateEvent(StateMachineEvent::INIT_EVENT),
  state(StateMachine::INIT),
  options(Options()), //default options
  loraTask(NULL),
  rxHandlingTask(NULL),
  stateTask(NULL),
  dataTask(NULL),
  sdTask(NULL),
  flashTask(NULL),
  loraRxQueue(NULL),
  loraTxQueue(NULL),
  sdQueue(NULL),
  flashQueue(NULL),
  espNowQueue(NULL),
  spiMutex(NULL),
  watchdogTimer(NULL),
  disconnectTimer(NULL)
  {}

//notify that changing state event occure
bool RocketControl::changeStateEvent(StateMachineEvent newEvent){
	//portENTER_CRITICAL(&stateLock);
  //out of range
  if(newEvent < IDLE_EVENT || newEvent > ABORT_EVENT){
    return false;
  }

  if((newEvent - 1) != stateEvent && newEvent != ABORT_EVENT){
    return false;
  }
    
  this->stateEvent = newEvent;
  //portEXIT_CRITICAL(&stateLock);
  xTaskNotifyGive(this->stateTask);
  return true;
}

//Use only in stateTask
void RocketControl::changeState(StateMachine newState){
  if((newState - 1) != state && newState != ABORT){
    return;
  }
    
  //portENTER_CRITICAL(&stateLock);
  this->state = newState;
  //portEXIT_CRITICAL(&stateLock);
}

void RocketControl::unsuccessfulEvent(){
  this->stateEvent = (StateMachineEvent)((uint8_t)stateEvent - 1);
}

void RocketControl::sendLog(char * message){
  static char log[SD_FRAME_ARRAY_SIZE] = {};
  char temp[40] = {};
  strcpy(log, "LOG ");
  snprintf(temp, 40, " [ %d , %lu ]\n", state, millis());
  strcat(log, message);
  strcat(log, temp);

  //Serial.println(log); //DEBUG
  xQueueSend(sdQueue, (void*)&log, 0);
}

uint32_t RocketControl::getDisconnectRemainingTime(){
  if(disconnectTimer == NULL){
    return disconnectDelay;
  }else if(xTimerIsTimerActive(disconnectTimer) == pdFALSE){
    return 0;
  }
  
  return (xTimerGetExpiryTime(disconnectTimer) - xTaskGetTickCount()) * portTICK_PERIOD_MS;
}
