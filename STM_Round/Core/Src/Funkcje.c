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

			if (strstr(xbee_rx.data_array, "R4TN") != NULL) {

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

			if (tfsStruct.maincompStringDma[5] == '2') gpsPeriod = 1000;
			else gpsPeriod = 8000;
		}

		if (strstr(tfsStruct.maincompStringDma, "TNWN;DSTA") != NULL) {

			ignite = 1;
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

	gpsPeriod = 1000;

	// Inity uartów:

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*) tfsStruct.maincompStringDma,
	RX_BUFFER_SIZE);

	xbee_init(&xbeeIgnition, 0x0013A20041A26FA2, &huart2);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) hallSensors, 5);

	// Inity struktury:

	tfsStruct.gpsFrameTimer = uwTick;
	tfsStruct.tanwaRxFlag = 0;
	tfsStruct.maincompRxFlag = 0;
}

/*******************************************************************************************/

void loraReaction(void) {

	// Przesyłanie wiadomości z LoRy do Maincompa:
	if (strstr(loraBuffer, "MNCP") != NULL) {
		HAL_UART_Transmit(&huart1, (uint8_t*) loraBuffer, strlen(loraBuffer),
				100);
	}

	// Przesyłanie wiadomości z LoRy do Tanwy:
	else if (strstr(loraBuffer, "TNWN") != NULL) {
		xbee_transmit_char(xbeeIgnition, loraBuffer);
	}

	else {
		badFrames++;
	}

	HAL_Delay(20);
	memset(loraBuffer, 0, BUFFER_SIZE);
}

/*******************************************************************************************/

#define DEL_TIME 1

void sendGPSData(void) {

	GPS_Process();

	int len = sprintf(tfsStruct.gpsStringLora,
			"R4GP;%.5f;%.5f;%.1f;%d;%d;%d;%d;%d;%d;%d\n",
			GPS.GPGGA.LatitudeDecimal, GPS.GPGGA.LongitudeDecimal,
			GPS.GPGGA.MSL_Altitude, GPS.GPGGA.SatellitesUsed, GPS.GPGGA.UTC_Sec,
			(int) hallSensors[0], (int) hallSensors[1], (int) hallSensors[2],
			(int) hallSensors[3], (int) badFrames);

	// Nie wysyłamy tutaj, bo wolimy mieć wszystko w jednej, wspólnej ramce.
	//loraSendData((uint8_t*) tfsStruct.gpsStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.gpsStringLora, len, 100);

	HAL_Delay(DEL_TIME);
}

/*******************************************************************************************/

void sendFromMaincompToLora(void) {

	char txString[250];
	strcpy(txString, tfsStruct.maincompStringLora);
	strcat(txString, tfsStruct.gpsStringLora);

	loraSendData((uint8_t*) txString, strlen(txString));
	HAL_Delay(DEL_TIME);
}

/*******************************************************************************************/

void sendFromTanwaToLora(void) {

	size_t len = strlen(tfsStruct.tanwaStringLora);

	loraSendData((uint8_t*) tfsStruct.tanwaStringLora, len);

	HAL_UART_Transmit(&huart1, (uint8_t*) tfsStruct.tanwaStringLora, len, 100);

	HAL_Delay(DEL_TIME);
}
