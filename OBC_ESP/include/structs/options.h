#ifndef OPTIONS_HH
#define OPTIONS_HH

//options are change only in stateTasks, and in handlingTask obviously
struct Options{
	uint16_t LoRaFrequencyMHz = 868;
  uint32_t countdownTime;
	int16_t ignitionTime; //ignition time
  uint8_t tankMinPressure; //bar
	bool flashWrite : 1;
	bool forceLaunch : 1; 
  //change in statTask

  uint16_t espnowSleepTime;
  uint16_t espnowShortPeriod;
  uint16_t espnowLongPeriod;
  TickType_t flashShortPeriod;
  TickType_t flashLongPeriod;
  TickType_t sdShortPeriod;
  TickType_t sdLongPeriod;
  TickType_t idlePeriod;
	TickType_t dataFramePeriod;
  TickType_t loraPeriod;
  TickType_t flashDataCurrentPeriod;
  TickType_t sdDataCurrentPeriod;
  
  Options():
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
};

#endif