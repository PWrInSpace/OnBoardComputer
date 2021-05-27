#include "Separation.h"

void initLoop(void) {

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

	if (uartRxFlag) {

		uartRxFlag = 0;

		if (strcmp(uartRxTab, "A1F") == 0)
			currentState = FLIGHT;
	}
	HAL_Delay(1);
}

/*****************************************************************/

void flightLoop(void) {

	if(!(AltiAP_GPIO_Port->IDR & AltiAP_Pin))
		doFirstSeparation(1000);

	if(!(AltiMA_GPIO_Port->IDR & AltiMA_Pin)) {

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

void doFirstSeparation(int emergencyTimeout) {

}

/*****************************************************************/

void doSecondSeparation(int emergencyTimeout) {

}

/*****************************************************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == USART2) {

		uartRxFlag = 1;
	}
}
