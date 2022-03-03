#include "missionTimer.h"

/**
 * @brief Construct a new Timer object
 * 
 */
Timer::Timer(): timer(0), enable(false){}

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
 * @return int32_t return time that has elapsed since startTimer, if timer is enable return NAN
 */
int32_t Timer::getTime() const{
  if(enable) return (millis() - timer);
  else return NAN;
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