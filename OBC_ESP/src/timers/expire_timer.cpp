#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/timers/expire_timer.h"

void ET_init(expire_timer_t *timer) {
    timer->duration_time = 0;
    timer->start_time = 0;
}


void ET_start(expire_timer_t *timer, miliseocnds duration) {
    assert(timer != NULL);
    timer->duration_time = duration;
    timer->start_time = pdTICKS_TO_MS(xTaskGetTickCount());
}

bool ET_is_expired(expire_timer_t *timer) {
    assert(timer != NULL);
    TickType_t start_time_ticks = pdMS_TO_TICKS(timer->start_time);
    TickType_t duration_ticks = pdMS_TO_TICKS(timer->duration_time);
    TickType_t present_ticks = xTaskGetTickCount();
    if (present_ticks - start_time_ticks < duration_ticks) {
        return false;
    }

    return true;
}

void ET_restart(expire_timer_t *timer) {
    assert(timer != NULL);
    timer->start_time = pdTICKS_TO_MS(xTaskGetTickCount());
}