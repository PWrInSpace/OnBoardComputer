#ifndef MISSION_TIMER_HH
#define MISSION_TIMER_HH

#include <stdint.h>
#include <Arduino.h>

class Timer{
  uint32_t timer;
  bool enable;

  public:
  Timer();
  void startTimer(uint32_t _timer);
  int getTime() const;
  bool isEnable() const;
  void turnOffTimer();
};


#endif