#ifndef BUZZER_H
#define BUZZER_H

#include <stdint.h>

void BUZZ_init(uint8_t pin);
void BUZZ_on(void);
void BUZZ_off(void);

#endif