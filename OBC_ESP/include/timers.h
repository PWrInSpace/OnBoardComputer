#ifndef TIMERS_HH
#define TIMERS_HH

#include "EEPROM.h"
#include "FreeRTOS.h"

#define WATCHDOG_EEPROM_SIZE 2

static const TickType_t watchdogDelay = 250 / portTICK_PERIOD_MS; //2s
static const TickType_t disconnectDelay = 15 * 60 * 1000 / portTICK_PERIOD_MS; //15min

struct WatchdogTimer{
  bool loraTaskFlag : 1;
  bool rxHandlingTaskFlag : 1;
  bool stateTaskFlag : 1;
  bool dataTaskFlag : 1;
  bool sdTaskFlag : 1;
  bool flashTaskFlag : 1;
  uint8_t resetCounter;
  uint8_t previousState;

  WatchdogTimer() = default;
  void begin();
  void EEPROMwrite(uint8_t state);
  uint8_t EEPROMread();
  void setFlags(bool state);
  void reset(uint8_t state);
};


#endif