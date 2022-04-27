#ifndef OPTIONS_STRUCT_HH
#define OPTIONS_STRUCT_HH

#include "../include/config.h"

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
  
  Options();
  void createOptionsFrame(char *data);
};

#endif