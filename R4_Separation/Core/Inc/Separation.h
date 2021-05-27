#ifndef INC_SEPARATION_H_
#define INC_SEPARATION_H_

#include "gpio.h"
#include "usart.h"
#include "StateMachine.h"
#include "string.h"

char uartRxTab[4];
_Bool uartRxFlag;

void initLoop(void);
void armedLoop(void);
void flightLoop(void);

_Bool firstSeparationRequirements(void);

_Bool secondSeparationRequirements(void);

void doFirstSeparation(int emergencyTimeout);

void doSecondSeparation(int emergencyTimeout);

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

#endif /* INC_SEPARATION_H_ */
