#include "Funkcje.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == _GPS_USART.Instance)
		GPS_CallBack();
}
