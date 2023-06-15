#include "../include/components/runcam.h"
#include <Arduino.h>

static struct{
    uint8_t gpio_pin;
}rc;

void RUNCAM_init(uint8_t pin){
    rc.gpio_pin = pin;
    pinMode(rc.gpio_pin, OUTPUT);
    digitalWrite(rc.gpio_pin, LOW);
}

void RUNCAM_turn_on(void){
    digitalWrite(rc.gpio_pin, HIGH);
}

void RUNCAM_turn_off(void){
    digitalWrite(rc.gpio_pin, LOW);
}