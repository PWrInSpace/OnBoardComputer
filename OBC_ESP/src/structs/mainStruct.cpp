#include "../include/structs/mainStructs.h"

/******   OPTIONS   *******/
Options::Options():
  LoRaFrequencyMHz(LoRa_FREQUENCY_MHZ),
  countdownTime(COUNTDOWN_TIME),
  ignitionTime(IGNITION_TIME),
  tankMinPressure(TANK_MIN_PRESSURE),
  flashWrite(FLASH_WRITE),
  forceLaunch(FORCE_LAUNCH),
  espnowSleepTime(ESP_NOW_SLEEP_TIME),
  espnowShortPeriod(ESP_NOW_SHORT_PERIOD),
  espnowLongPeriod(ESP_NOW_LONG_PERIOD),
  flashShortPeriod(FLASH_SHORT_PERIOD),
  flashLongPeriod(FLASH_LONG_PERIOD),
  sdShortPeriod(SD_SHORT_PERIOD),
  sdLongPeriod(SD_LONG_PERIOD),
  idlePeriod(IDLE_PERIOD),
  dataFramePeriod(DATA_PERIOD),
  loraPeriod(LORA_PERIOD),
  flashDataCurrentPeriod(FLASH_LONG_PERIOD),
  sdDataCurrentPeriod(SD_LONG_PERIOD)
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
