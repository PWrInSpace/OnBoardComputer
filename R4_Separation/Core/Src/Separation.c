#include "Separation.h"

void initLoop(void) {

	if (uwTick - timer > 5000) {
		timer = uwTick;
		sendTestData();
	}

	HAL_Delay(1);
}

/*****************************************************************/

void flightLoop(void) {

	if (uwTick - timer > 250) {
		timer = uwTick;
		sendTestData();
	}

	if (!Sep1Done && AltiAP_GPIO_Port->IDR & AltiAP_Pin)
		doFirstSeparation();

	if (Sep1Done && AltiMA_GPIO_Port->IDR & AltiMA_Pin) {

		doSecondSeparation(3000);
		currentState = END;
	}

	HAL_Delay(1);
}

/*****************************************************************/

void stopAll(void) {

	LED_GPIO_Port->ODR |= LED_Pin;
	HAL_SuspendTick();

}

/*****************************************************************/

void doFirstSeparation() {

	timer = uwTick;

	Separ1A_GPIO_Port->ODR |= Separ1A_Pin;
	Separ1B_GPIO_Port->ODR |= Separ1B_Pin;

	HAL_Delay(500);
	while (P1Test_GPIO_Port->IDR & P1Test_Pin) {

		if (uwTick - timer > 2000)
			break;
	}

	Separ1A_GPIO_Port->ODR &= ~Separ1A_Pin;
	Separ1B_GPIO_Port->ODR &= ~Separ1B_Pin;

	sendTestData();
	HAL_Delay(150);
	HAL_UART_Transmit(&huart2, (uint8_t*) "A1S1", 4, 500);
}

/*****************************************************************/

void doSecondSeparation(int emergencyTimeout) {

	timer = uwTick;

	// Tutaj dodać odpowiednie ustawienie serwa
	//__HAL_TIM_SET_COMPARE(&htim3, SEPAR_2_PWM_CHANNEL, valu);

	while (1/*P2Test_GPIO_Port->IDR & P2Test_Pin*/) {

		if (uwTick - timer > emergencyTimeout) {

			Separ2AW_GPIO_Port->ODR |= Separ2AW_Pin;
			HAL_Delay(2000);
			Separ2AW_GPIO_Port->ODR &= Separ2AW_Pin;

			break;
		}
	}

	sendTestData();
	HAL_Delay(150);
	HAL_UART_Transmit(&huart2, (uint8_t*) "A1S2", 4, 500);
}

/*****************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART2) {

		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);

		if (strstr(uartRxTab, "LECI") != NULL)
			currentState = FLIGHT;
		else if (strstr(uartRxTab, "END") != NULL)
			currentState = END;

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) uartRxTab, ARRAY_SIZE);
	}
}

/*****************************************************************/

void sendTestData(void) {

	char messTx[15];
	P1Test = P1Test_GPIO_Port->IDR & P1Test_Pin;
	P2Test = P2Test_GPIO_Port->IDR & P2Test_Pin;

	sprintf(messTx, "A1DAT%d%d", P1Test, P2Test);

	LED_GPIO_Port->ODR ^= LED_Pin;
	HAL_UART_Transmit(&huart2, (uint8_t*) messTx, strlen(messTx), 500);
}
