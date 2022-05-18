#ifndef OPTIONS_HH
#define OPTIONS_HH

//options are change only in stateTasks, and in handlingTask obviously
struct Options{
	uint16_t LoRaFrequencyMHz = 868;
  uint32_t countdownTime;
	int32_t ignitionTime; //ignition time
  uint8_t tankMinPressure; //bar
	bool flashWrite : 1;
	bool forceLaunch : 1;

	TickType_t dataCurrentPeriod;
  TickType_t loraCurrentPeriod;
  TickType_t flashDataCurrentPeriod;
  TickType_t sdDataCurrentPeriod;
  
  Options():
    LoRaFrequencyMHz(LoRa_FREQUENCY_MHZ),
    countdownTime(COUNTDOWN_TIME),
    ignitionTime(IGNITION_TIME),
    tankMinPressure(TANK_MIN_PRESSURE),
    flashWrite(FLASH_WRITE),
    forceLaunch(FORCE_LAUNCH),
    dataCurrentPeriod(DATA_PERIOD),
    loraCurrentPeriod(IDLE_PERIOD),
    flashDataCurrentPeriod(FLASH_LONG_PERIOD),
    sdDataCurrentPeriod(IDLE_PERIOD)
    {}
};

#endif