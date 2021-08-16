#include "Timer_ms.h"

void Timer_ms::reset() {

    this->timer = millis();
}

bool Timer_ms::check() {

    if (millis() - this->timer > this->val) {
        
        this->reset();
        this->flag = true;
        return true;
    }

    return false;
}

void Timer_ms::setVal(uint16_t val) {

    this->val = val;
}