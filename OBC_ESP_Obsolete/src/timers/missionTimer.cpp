#include "../include/timers/missionTimer.h"

/**
 * @brief Construct a new MissionTimer object
 * 
 */
MissionTimer::MissionTimer(): tzero_time(0), enable(false), disableValue(-999){}

/**
 * @brief turn on timer
 * 
 * @param countdown_begin_time negative number in miliseconds
 */
void MissionTimer::startTimer(int64_t countdown_begin_time){
  assert(countdown_begin_time < 0);
  tzero_time = millis() - countdown_begin_time;
  enable = true;
}

/**
 * @brief get time
 * 
 * @return int32_t return time that has elapsed since startTimer, if timer is enable return disable value
 */
int MissionTimer::getTime() const{
  if (enable == true) {
    return (millis() - tzero_time);
  }
  
  return disableValue;
}

/**
 * @brief check if timer is enable
 * 
 * @return true timer running
 * @return false timer disable
 */
bool MissionTimer::isEnable() const{
  return enable;
}

/**
 * @brief turn off timer
 * 
 */
void MissionTimer::turnOffTimer(){
  tzero_time = 0;
  enable = false;
}

/**
 * @brief set disable value
 * 
 * @param _disableValue time or sth
 */
void MissionTimer::setDisableValue(int64_t _disableValue){
  disableValue = _disableValue;
}