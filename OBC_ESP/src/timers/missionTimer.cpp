#include "../include/timers/missionTimer.h"

/**
 * @brief Construct a new Timer object
 * 
 */
Timer::Timer(): timer(0), enable(false), disableValue(-999){}

/**
 * @brief turn on timer
 * 
 * @param _timer timer start time
 */
void Timer::startTimer(uint32_t _timer){
  timer = _timer;
  enable = true;
}

/**
 * @brief get time
 * 
 * @return int32_t return time that has elapsed since startTimer, if timer is enable return disable value
 */
int Timer::getTime() const{
  if(enable) return (millis() - timer);
  else return disableValue;
}

/**
 * @brief check if timer is enable
 * 
 * @return true timer running
 * @return false timer disable
 */
bool Timer::isEnable() const{
  return enable;
}

/**
 * @brief turn off timer
 * 
 */
void Timer::turnOffTimer(){
  timer = 0;
  enable = false;
}

/**
 * @brief set disable value
 * 
 * @param _disableValue time or sth
 */
void Timer::setDisableValue(uint32_t _disableValue){
  disableValue = _disableValue;
}