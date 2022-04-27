#include "../include/structs/optionsStruct.h"

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

/*** OPTONS FRAME ***/
void Options::createOptionsFrame(char* data){
  size_t length;

  strcpy(data, LORA_TX_OPTIONS_PREFIX);

  length = snprintf(NULL, 0, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
    LoRaFrequencyMHz, countdownTime, ignitionTime,
    tankMinPressure, flashWrite, forceLaunch,
    espnowSleepTime, espnowShortPeriod, espnowLongPeriod,
    flashShortPeriod, flashLongPeriod, 
    sdShortPeriod, sdLongPeriod, idlePeriod, dataFramePeriod, 
    loraPeriod, flashDataCurrentPeriod, sdDataCurrentPeriod);

  char opt[length];

  snprintf(opt, length, "%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d\n",
    LoRaFrequencyMHz, countdownTime, ignitionTime,
    tankMinPressure, flashWrite, forceLaunch,
    espnowSleepTime, espnowShortPeriod, espnowLongPeriod,
    flashShortPeriod, flashLongPeriod, 
    sdShortPeriod, sdLongPeriod, idlePeriod, dataFramePeriod, 
    loraPeriod, flashDataCurrentPeriod, sdDataCurrentPeriod);

  strcat(data, opt);
}