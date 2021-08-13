#ifndef TIMER_MS_H
#define TIMER_MS_H

#include <Arduino.h>

class Timer_ms {

    uint32_t timer;
    uint16_t val;

    public:

    Timer_ms() { timer = millis(); }

    void reset();
    bool check();
    void setVal(uint16_t val);

};

#endif