#ifndef INC_SEPARATION_H_
#define INC_SEPARATION_H_

#include "gpio.h"
#include "usart.h"
#include "StateMachine.h"
#include "string.h"

#define SEPAR_2_PWM_CHANNEL TIM_CHANNEL_2

char uartRxTab[4];
_Bool uartRxFlag;

uint32_t timer;

void initLoop(void);

void armedLoop(void);

void flightLoop(void);

void stopAll(void);

void doFirstSeparation(int emergencyTimeout);

void doSecondSeparation(int emergencyTimeout);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_SEPARATION_H_ */
