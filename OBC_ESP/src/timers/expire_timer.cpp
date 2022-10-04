#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "../include/timers/expire_timer.h"

void ET_init(expire_timer_t *timer) {
    timer->duration_time = 0;
    timer->start_time = 0;
    timer->end_time = 0;
    timer->previous_start_time = 0;
}

static void overflow_detector(expire_timer_t *timer) {
    if (timer->end_time < timer->start_time) {
        Serial.println("Overflow detected");
        Serial.print("Duration time: ");
        Serial.println(timer->duration_time);
        Serial.print("Start time: ");
        Serial.println(timer->start_time);
        Serial.print("End time: ");
        Serial.println(timer->end_time);
    }
}

static void check_previous_time(expire_timer_t *timer) {
    if (timer->previous_start_time > timer->start_time) {
        Serial.println("Previous start time is bigger than present time");
    }

    timer->previous_start_time = timer->start_time;
}


void ET_start(expire_timer_t *timer, miliseconds duration) {
    assert(timer != NULL);
    timer->duration_time = duration;
    timer->start_time = pdTICKS_TO_MS(xTaskGetTickCount());
    timer->end_time = timer->start_time + timer->duration_time;
    overflow_detector(timer);
    check_previous_time(timer);
}

bool ET_is_expired(expire_timer_t *timer) {
    assert(timer != NULL);
    TickType_t start_time_ticks = pdMS_TO_TICKS(timer->start_time);
    TickType_t duration_ticks = pdMS_TO_TICKS(timer->duration_time);
    TickType_t present_ticks = xTaskGetTickCount();
    TickType_t end_ticks = pdMS_TO_TICKS(timer->end_time);
    // if (present_ticks - start_time_ticks < duration_ticks) {
    //     return false;
    // }
    if (end_ticks > present_ticks) {
        return false;
    }

    return true;
}

void ET_restart(expire_timer_t *timer) {
    assert(timer != NULL);
    timer->start_time = pdTICKS_TO_MS(xTaskGetTickCount());
}