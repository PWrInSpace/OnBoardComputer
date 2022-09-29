#ifndef EXPIRE_TIMER_HH
#define EXPIRE_TIMER_HH

#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>

typedef uint32_t miliseocnds;
typedef uint32_t seconds;

typedef struct {
    miliseocnds duration_time;
    miliseocnds start_time;
} expire_timer_t;

void ET_init(expire_timer_t *timer);
void ET_start(expire_timer_t *timer, miliseocnds duration);
bool ET_is_expired(expire_timer_t *timer);
void ET_restart(expire_timer_t *timer);

#endif