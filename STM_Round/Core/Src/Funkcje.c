#include "Funkcje.h"

_Bool cmeaSent;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == _GPS_USART.Instance)
		GPS_CallBack();

	/************************************************/

	if (huart->Instance == USART2) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);

		xbee_receive();
		if (xbee_rx.data_flag) {

			//if (strstr(xbee_rx.data_array, "DDAT") != NULL)
			// JAKIŚ KOD TODO

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}

	/************************************************/

	if (huart->Instance == USART1) {

		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);

		// JAKIS KOD TODO!!!

		memset(timersFlagsStrings.maincompString, 0, RX_BUFFER_SIZE);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) timersFlagsStrings.maincompString, RX_BUFFER_SIZE);
	}

}

/*******************************************************************************************/

void initAll(void) {

	loraInit();
	GPS_Init();

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*) timersFlagsStrings.maincompString, RX_BUFFER_SIZE);

	xbee_init(&xbeeIgnition, 0x0013A20041A26FA2, &huart2);

	timersFlagsStrings.gpsFrameTimer = uwTick;
}

/*******************************************************************************************/

void logAndSendDataLoop(void) {

	GPS_Process();

	logDataLoop();

	//loraSendData((uint8_t*) bufferLoraTx, strlen(bufferLoraTx));

	// To trzeba jakoś sprytnie przerobić, by wykryć, kiedy stracimy połączenie:
	//if (uwTick - timers.checkConnectionTimer > 5500) otherData.ignitionState = 0;

}

/*******************************************************************************************/

void logDataLoop(void) {

	// Trzeba przerobić na sam GPS:
	/*sprintf(bufferLoraTx,
			"ADAT;%d;%.5f;%.5f;%.1f;%d:%d:%d;%.3f;%d;%d;%.2f;%.2f;%d;%d\n",
			(int) rocketState, GPS.GPGGA.LatitudeDecimal,
			GPS.GPGGA.LongitudeDecimal, GPS.GPGGA.MSL_Altitude,
			GPS.GPGGA.UTC_Hour, GPS.GPGGA.UTC_Min, GPS.GPGGA.UTC_Sec,
			GPS.GPGGA.HDOP, GPS.GPGGA.SatellitesUsed, otherData.sdState,
			otherData.pitotStatic, otherData.pitotDynamic,
			otherData.computedAltitude, otherData.ignitionState);*/
}

/*******************************************************************************************/

_Bool cmeaSent;

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

	memset(loraBuffer, 0, BUFFER_SIZE);
}

