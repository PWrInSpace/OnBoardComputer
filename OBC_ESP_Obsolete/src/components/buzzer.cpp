#include "../include/components/buzzer.h"
#include <Arduino.h>

#define BUZZER_HIGH 1
#define BUZZER_LOW 0

static struct {
    uint8_t pin;
    uint8_t state;
}buzz;

void BUZZ_init(uint8_t pin) {
    buzz.pin = pin;
    buzz.state = BUZZER_LOW;
    pinMode(buzz.pin, OUTPUT);
    digitalWrite(buzz.pin, buzz.state);
}

void BUZZ_on(void) {
    buzz.state = BUZZER_HIGH;
    digitalWrite(buzz.pin, BUZZER_HIGH);
}

void BUZZ_off(void) {
    buzz.state = BUZZER_LOW;
    digitalWrite(buzz.pin, BUZZER_LOW);
}