#include "../include/timers/watchdog.h"

//watchdog timer struct
/*
WatchdogTimer::WatchdogTimer() : 
  LoRaTaskFlag(false),
  rxHandlingTaskFlag(false),
  stateTaskFlag(false),
  dataTaskFlag(false),
  sdTaskFlag(false),
  flashTaskFlag(false),
  resetCounter(0),
  previousState(0) {}
*/

void WatchdogTimer::begin(){
  EEPROM.begin(WATCHDOG_EEPROM_SIZE);
}

void WatchdogTimer::EEPROMwrite(uint8_t state){
  EEPROM.write(0, resetCounter);
  EEPROM.write(1, state);
  
  EEPROM.commit();
}

uint8_t WatchdogTimer::EEPROMread(){
  resetCounter = EEPROM.read(0);
  previousState = EEPROM.read(1);

  //EEPROM.write(0, 0); //reset resetCounter
  EEPROM.write(1, 0); //set INIT state (reset previousState eeprom data - INIT - default)
  EEPROM.commit();

  return previousState;
}

void WatchdogTimer::setFlags(bool state){
  loraTaskFlag = state;
  rxHandlingTaskFlag = state;
  stateTaskFlag = state;
  dataTaskFlag = state;
  sdTaskFlag = state;
  flashTaskFlag = state;
}

void WatchdogTimer::reset(uint8_t state){
  Serial.println("WATCHDOG RESET"); //DEBUG
  delay(1000);
  resetCounter += 1;
  EEPROMwrite(state);
  ESP.restart();
}