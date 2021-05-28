#include "Separation.h"

void initLoop(void) {

	uint32_t timer = uwTick;

	if (uwTick - timer > 500) {
		timer = uwTick;
		LED_GPIO_Port->ODR ^= LED_Pin;
	}

	if (uartRxFlag) {

		uartRxFlag = 0;

		if (strcmp(uartRxTab, "A1A") == 0)
			currentState = ARMED;

		else if (strcmp(uartRxTab, "A1I") == 0) {

			char mess[4] = "AIN";
			HAL_UART_Transmit(&huart2, (uint8_t*) mess, strlen(mess), 100);
		}

	}
	HAL_Delay(1);
}

/*****************************************************************/

void armedLoop(void) {

	uint32_t timer = uwTick;

	if (uwTick - timer > 250) {
		timer = uwTick;
		LED_GPIO_Port->ODR ^= LED_Pin;
	}

	if (uartRxFlag) {

		uartRxFlag = 0;

		if (strcmp(uartRxTab, "A1F") == 0)
			currentState = FLIGHT;
	}
	HAL_Delay(1);
}

/*****************************************************************/

void flightLoop(void) {

	uint32_t timer = uwTick;

	if (uwTick - timer > 100) {
		timer = uwTick;
		LED_GPIO_Port->ODR ^= LED_Pin;
	}

	if (!(AltiAP_GPIO_Port->IDR & AltiAP_Pin))
		doFirstSeparation(1000);

	if (!(AltiMA_GPIO_Port->IDR & AltiMA_Pin)) {

		doSecondSeparation(1000);
		currentState = END;
	}

	if (uartRxFlag) {

		uartRxFlag = 0;

		if (strcmp(uartRxTab, "A1G") == 0)
			doFirstSeparation(1000);

		if (strcmp(uartRxTab, "A1M") == 0) {

			doSecondSeparation(1000);
			currentState = END;
		}

	}
	HAL_Delay(1);
}

/*****************************************************************/

void stopAll(void) {

	LED_GPIO_Port->ODR |= LED_Pin;


}

/*****************************************************************/

void doFirstSeparation(int emergencyTimeout) {

	uint32_t timer = uwTick;

	Separ1A_GPIO_Port->ODR |= Separ1A_Pin;
	Separ1B_GPIO_Port->ODR |= Separ1B_Pin;

	while (P1Test_GPIO_Port->IDR & P1Test_Pin) {

		if (uwTick - timer > emergencyTimeout)
			break;
	}

	Separ1A_GPIO_Port->ODR &= ~Separ1A_Pin;
	Separ1B_GPIO_Port->ODR &= ~Separ1B_Pin;
}

/*****************************************************************/

void doSecondSeparation(int emergencyTimeout) {

	uint32_t timer = uwTick;

	// Tutaj dodać odpowiednie ustawienie serwa
	//__HAL_TIM_SET_COMPARE(&htim3, SEPAR_2_PWM_CHANNEL, valu);

	while (P2Test_GPIO_Port->IDR & P2Test_Pin) {

		if (uwTick - timer > emergencyTimeout) {

			Separ2AW_GPIO_Port->ODR |= Separ2AW_Pin;
			HAL_Delay(2000);
			Separ2AW_GPIO_Port->ODR &= Separ2AW_Pin;

			break;
		}
	}

}

/*****************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART2) {

		uartRxFlag = 1;

		HAL_UART_Receive_IT(&huart2, (uint8_t*) uartRxTab, 4);
	}
}
