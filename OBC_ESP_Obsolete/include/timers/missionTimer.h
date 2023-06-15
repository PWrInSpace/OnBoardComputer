#ifndef MISSION_TIMER_HH
#define MISSION_TIMER_HH

#include <stdint.h>
#include <Arduino.h>

class  MissionTimer{
  int64_t tzero_time;
  bool enable;
  int64_t disableValue;

  public:
  MissionTimer();
  void startTimer(int64_t _timer);
  int getTime() const;
  bool isEnable() const;
  void turnOffTimer();
  void setDisableValue(int64_t _disableValue);
};


#endif