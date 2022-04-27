#include "../include/structs/mainStructs.h"

/*
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


RocketControl::RocketControl():
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

void RocketControl::sendLog(char * message){
  static char log[SD_FRAME_ARRAY_SIZE] = {};
  char temp[40] = {};
  strcpy(log, "LOG ");  
  snprintf(temp, 40, " [ %d , %lu ]\n", /*state1, millis());
  strcat(log, message);
  strcat(log, temp);

  //Serial.println(log); //DEBUG
  //xQueueSend(sdQueue, (void*)&log, 0);
}

uint32_t RocketControl::getDisconnectRemainingTime(){
  if(disconnectTimer == NULL){
    return disconnectDelay;
  }else if(xTimerIsTimerActive(disconnectTimer) == pdFALSE){
    return 0;
  }
  
  return (xTimerGetExpiryTime(disconnectTimer) - xTaskGetTickCount()) * portTICK_PERIOD_MS;
}

*/