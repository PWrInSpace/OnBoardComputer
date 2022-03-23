#include "Funkcje.h"

#define DEL_TIME 1

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

				int tanWaState, tanWaIgniter, tanWaFill, tanWaDepr, tanWaUpust;
				int test;
				sscanf(xbee_rx.data_array, "R4TN;%f;%d;%d;%d;%d;%d;%d;%f;%f",
						&dataFrame.tanWaVoltage, 	&tanWaState,
						&tanWaIgniter, 				&tanWaFill,
						&tanWaDepr, 				&tanWaUpust, &test,
						&dataFrame.rocketWeightKg, 	&dataFrame.tankWeightKg);

				dataFrame.tanWaState 	= tanWaState;
				dataFrame.tanWaIgniter 	= tanWaIgniter;
				dataFrame.tanWaFill 	= tanWaFill;
				dataFrame.tanWaDepr 	= tanWaDepr;
				dataFrame.tanWaUpust 	= tanWaUpust;
				if (tanWaState == 3) tfsStruct.launched = 1;
			}

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}

	/************************************************/

	if (huart->Instance == USART1) {

		HAL_UART_Receive_DMA(&huart1, (uint8_t*) &tfsStruct.espBinData, sizeof(EspBinData));

		//memcpy(&tfsStruct.espBinData, tfsStruct.stringFromESP, sizeof(EspBinData));
		dataFrame.batteryV = tfsStruct.espBinData.batteryV;
		dataFrame.tankPressure = tfsStruct.espBinData.tankPressure;
	}
}

/*******************************************************************************************/

void initAll(void) {

	loraInit();
	GPS_Init();

	// Inity uartów:

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	HAL_UART_Receive_DMA(&huart1, (uint8_t*) &tfsStruct.espBinData, sizeof(EspBinData));

	xbee_init(&xbeeIgnition, 0x0013A20041A26FDD, &huart2);

	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) dataFrame.hallSensors, 5);

	// Inity struktury:
	tfsStruct.frameTimer = uwTick;
	tfsStruct.saveTimer = uwTick;
}

/*******************************************************************************************/

void loraReaction(void) {

	// Przesyłanie wiadomości z LoRy do Tanwy:
	if (strstr(loraBuffer, "TNWN") != NULL) {

		xbee_transmit_char(xbeeIgnition, loraBuffer);
	}

	HAL_Delay(20);
	memset(loraBuffer, 0, BUFFER_SIZE);
}

/*******************************************************************************************/

void generateAndSendFrame(void) { // TODO

	char txString[250];

	sprintf(txString, "R4OR;%d;%f;%f;%f;%f;%f;%d;%d;%d;%d;%d;%d;%d;%f;%d;%d;%d;%d;%d;%f;%f;%d;%d\n",
			(int)dataFrame.time_ms, 	dataFrame.batteryV, 		dataFrame.tankPressure,
			dataFrame.gpsLatitude, 		dataFrame.gpsLongitude, 	dataFrame.gpsAltitude,
			dataFrame.gpsSatNum, 		dataFrame.gpsTimeS,			dataFrame.hallSensors[0],
			dataFrame.hallSensors[1], 	dataFrame.hallSensors[2],	dataFrame.hallSensors[3],
			dataFrame.hallSensors[4], 	dataFrame.tanWaVoltage,		dataFrame.tanWaState,
			dataFrame.tanWaIgniter,		dataFrame.tanWaFill,		dataFrame.tanWaDepr,
			dataFrame.tanWaUpust, 		dataFrame.tankWeightKg, 	dataFrame.rocketWeightKg,
			(int)dataFrame.rocketWRaw, 	(int)dataFrame.tankWRaw);

	loraSendData((uint8_t*) txString, strlen(txString));
	HAL_Delay(DEL_TIME);
}

/*******************************************************************************************/

void updateFrame(void) {

	dataFrame.time_ms = uwTick;
	dataFrame.gpsLatitude  = GPS.GPGGA.LatitudeDecimal;
	dataFrame.gpsLongitude = GPS.GPGGA.LongitudeDecimal;
	dataFrame.gpsAltitude  = GPS.GPGGA.MSL_Altitude;
	dataFrame.gpsSatNum    = GPS.GPGGA.SatellitesUsed;
	dataFrame.gpsTimeS     = GPS.GPGGA.UTC_Sec;

	if (tfsStruct.launched) {
		HAL_UART_Transmit(&huart1, (uint8_t*) &dataFrame, sizeof(dataFrame), 100);
	}
}
