#include "Funkcje.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == _GPS_USART.Instance)
		GPS_CallBack();

	/************************************************/

	if (huart->Instance == USART2) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);

		xbee_receive();
		if (xbee_rx.data_flag) {

			if (strstr(xbee_rx.data_array, "RAMKA_TANWY_TODO") != NULL) {

				tfsStruct.tanwaRxFlag = 1;
				strcpy(tfsStruct.tanwaStringLora, xbee_rx.data_array);
			}

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}

	/************************************************/

	if (huart->Instance == USART1) {

		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);

		if (strstr(tfsStruct.maincompStringDma, "R4MC") != NULL) {

			tfsStruct.maincompRxFlag = 1;
			strcpy(tfsStruct.maincompStringLora, tfsStruct.maincompStringDma);
		}

		memset(tfsStruct.maincompStringDma, 0, RX_BUFFER_SIZE);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) tfsStruct.maincompStringDma,
		RX_BUFFER_SIZE);
	}

}

/*******************************************************************************************/

void initAll(void) {

	loraInit();
	GPS_Init();

	// Inity uartów:

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*) tfsStruct.maincompStringDma,
	RX_BUFFER_SIZE);

	xbee_init(&xbeeIgnition, 0x0013A20041A26FA2, &huart2);

	// Inity struktury:

	tfsStruct.gpsFrameTimer = uwTick;
	tfsStruct.tanwaRxFlag = 0;
	tfsStruct.maincompRxFlag = 0;
}

/*******************************************************************************************/

void loraReaction(void) {

	// Trzeba przerobić:
	/*if (strstr(loraBuffer, "STAT") != NULL && strlen(loraBuffer) >= 8) {

	 if (rocketState == (loraBuffer[5] - '0')) {
	 rocketState = loraBuffer[7] - '0';
	 xbee_transmit_char(xbeeIgnition, loraBuffer);
	 if (rocketState == FIRST_SEPAR) HAL_UART_Transmit(&huart1, (uint8_t*) "FORCE1", 6, 500);
	 else if (rocketState == SECOND_SEPAR) HAL_UART_Transmit(&huart1, (uint8_t*) "FORCE2", 6, 500);
	 }
	 }

	 else if (strstr(loraBuffer, "ABRT") != NULL) {

	 rocketState = ABORT;
	 HAL_UART_Transmit(&huart1, (uint8_t*) "END", 3, 500);
	 xbee_transmit_char(xbeeIgnition, "STAT;-;7");
	 }*/

	HAL_Delay(10);
	memset(loraBuffer, 0, BUFFER_SIZE);
}

/*******************************************************************************************/

void sendGPSData(void) {

	GPS_Process();

	int len = sprintf(tfsStruct.gpsStringLora, "R4GP;%.5f;%.5f;%.1f;%d\n",
			GPS.GPGGA.LatitudeDecimal, GPS.GPGGA.LongitudeDecimal,
			GPS.GPGGA.MSL_Altitude, GPS.GPGGA.SatellitesUsed);

	loraSendData((uint8_t*) tfsStruct.gpsStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.gpsStringLora, len, 100);

	HAL_Delay(100);
}

/*******************************************************************************************/

void sendFromMaincompToLora(void) {

	loraSendData((uint8_t*) tfsStruct.maincompStringLora,
			strlen(tfsStruct.maincompStringLora));
	HAL_Delay(100);
}

/*******************************************************************************************/

void sendFromTanwaToLora(void) {

	size_t len = strlen(tfsStruct.tanwaStringLora);

	loraSendData((uint8_t*) tfsStruct.tanwaStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.tanwaStringLora, len, 100);

	HAL_Delay(100);
}
