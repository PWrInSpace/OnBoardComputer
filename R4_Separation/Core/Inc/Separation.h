#ifndef INC_SEPARATION_H_
#define INC_SEPARATION_H_

#include "gpio.h"
#include "usart.h"
#include "StateMachine.h"
#include "string.h"
#include <stdio.h>

#define SEPAR_2_PWM_CHANNEL TIM_CHANNEL_2
#define ARRAY_SIZE 10
_Bool P1Test;
_Bool P2Test;

char uartRxTab[ARRAY_SIZE];

uint32_t timer;

void sendTestData(void);

void initLoop(void);

void flightLoop(void);

void stopAll(void);

void doFirstSeparation(void);

void doSecondSeparation(int emergencyTimeout);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_SEPARATION_H_ */
