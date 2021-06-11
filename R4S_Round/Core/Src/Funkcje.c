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

			if (strstr(xbee_rx.data_array, "DDAT") != NULL) {

				strcat(xbee_rx.data_array, "\n");
				loraSendData((uint8_t*) xbee_rx.data_array,
						strlen(xbee_rx.data_array));
			} else if (strstr(xbee_rx.data_array, "AME1")) {

				// Zapis danych z pitota do zmiennnych TODO!!!
				sscanf(xbee_rx.data_array ,"AME1;%f;%f", &otherData.pitotDynamic, &otherData.pitotStatic);


			} else if (strstr(xbee_rx.data_array, "AMEA")) {

				// Zapis danych z pitota do zmiennnych TODO!!!
				sscanf(xbee_rx.data_array ,"AME1;%f;%f", &otherData.pitotDynamic, &otherData.pitotStatic);

				cmeaSent = 1;
				if (rocketState == FIRST_SEPAR)
					xbee_transmit_char(xbeePrandl, "CSTP");
			} else if (strstr(xbee_rx.data_array, "ASTB")) {
				ignitionConfirmation = 1;
			}

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}

	/************************************************/

	if (huart->Instance == USART1) {

		__HAL_UART_CLEAR_IDLEFLAG(&huart1);
		HAL_UART_DMAStop(&huart1);

		if (strstr(separationBufferRx, "A1DAT") != NULL) {

			otherData.separationTest1 = separationBufferRx[5] - '0';
			otherData.separationTest2 = separationBufferRx[6] - '0';
		}
		else if (rocketState == FLIGHT && strstr(separationBufferRx, "A1S1") != NULL) {

			rocketState = FIRST_SEPAR;
		}
		else if (rocketState == FLIGHT && strstr(separationBufferRx, "A1S2") != NULL) {

			rocketState = SECOND_SEPAR;
		}

		memset(separationBufferRx, 0, 10);
		HAL_UART_Receive_DMA(&huart1, (uint8_t*) separationBufferRx, 10);
	}

}

/*******************************************************************************************/

void initAll(void) {

	timers.launchTimer = 40;
	rocketState = INIT;
	loraInit();
	GPS_Init();

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);

	__HAL_UART_ENABLE_IT(&huart1, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart1, (uint8_t*) separationBufferRx, 10);

	xbee_init(&xbeePrandl, 0x0013A20041A26FDD, &huart2);
	xbee_init(&xbeeIgnition, 0x0013A20041A26FA2, &huart2);

	sd_spi_init();
	f_open(&file, "plik.txt", FA_WRITE | FA_OPEN_APPEND);
	f_close(&file);

	timers.logDataTimer = uwTick;
	timers.sendDataTimer = uwTick;
	timers.tenSecondTimer = uwTick;
}

/*******************************************************************************************/

void testPrandl(void) {

	//xbee_transmit_char(xbeePrandl, "CME1");
}

/*******************************************************************************************/

void logAndSendDataLoop(void) {

	GPS_Process();

	//otherData.sdState = !otherData.sdState;

	logDataLoop();

	loraSendData((uint8_t*) bufferLoraTx, strlen(bufferLoraTx));

}

/*******************************************************************************************/

void logDataLoop(void) {

	sprintf(bufferLoraTx,
			"ADAT;%d;%.5f;%.5f;%.1f;%d:%d:%d;%.3f;%d;%d;%.2f;%.2f;%d;%d;%d;%d\n",
			(int) rocketState, GPS.GPGGA.LatitudeDecimal,
			GPS.GPGGA.LongitudeDecimal, GPS.GPGGA.MSL_Altitude,
			GPS.GPGGA.UTC_Hour, GPS.GPGGA.UTC_Min, GPS.GPGGA.UTC_Sec,
			GPS.GPGGA.HDOP, GPS.GPGGA.SatellitesUsed, otherData.sdState,
			otherData.pitotStatic, otherData.pitotDynamic,
			otherData.computedAltitude, otherData.computedSpeed,
			otherData.separationTest1, otherData.separationTest2);

	f_open(&file, "plik.txt", FA_WRITE | FA_OPEN_APPEND | FA_READ);

	f_lseek(&file, allWrittenBytes);
	int bytWrot = f_puts(bufferLoraTx, &file);

	if (bytWrot > 0)
		otherData.sdState = 1;
	else
		otherData.sdState = 0;

	allWrittenBytes += bytWrot;
	f_close(&file);
}

/*******************************************************************************************/

_Bool cmeaSent;

void setPeriods(void) {

	switch (rocketState) {

	case INIT:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;

	case IDLE:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;
	case ARMED_HARD:
		break;

	case ARMED_SOFT:
		break;

	case READY:

		timers.sendDataPeriod = 2500;
		break;

	case FLIGHT:

		timers.sendDataPeriod = 2000;
		timers.logDataPeriod = 50;

		if (!cmeaSent) {

			xbee_transmit_char(xbeePrandl, "CMEA");
			HAL_Delay(200);
		}
		break;

	case ABORT:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;

	case FIRST_SEPAR:

		timers.sendDataPeriod = 2000;
		timers.logDataPeriod = 200;
		xbee_transmit_char(xbeePrandl, "CSTP");
		break;

	case SECOND_SEPAR:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 500;
		if(GPS.GPGGA.MSL_Altitude < 100) rocketState = END;
		break;

	case END:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;
	}
}

/*******************************************************************************************/

void loraReaction(void) {

	if (strstr(loraBuffer, "STAT") != NULL && strlen(loraBuffer) >= 8) {

		if (rocketState == (loraBuffer[5] - '0') && (loraBuffer[5] - '0') < 6) {
			rocketState = loraBuffer[7] - '0';
			xbee_transmit_char(xbeeIgnition, loraBuffer);
		}
	}

	else if (strstr(loraBuffer, "ABRT") != NULL) {

		rocketState = ABORT;
		HAL_UART_Transmit(&huart1, (uint8_t*) "END", 3, 500);
		xbee_transmit_char(xbeeIgnition, "STAT;-;7");
	}

	memset(loraBuffer, 0, BUFFER_SIZE);
}

/*******************************************************************************************/

void doLaunch(void) {

	rocketState = FLIGHT;
	setPeriods();
	char mess[] = "LECI";
	HAL_UART_Transmit(&huart1, (uint8_t*) mess, strlen(mess), 500);

	if (!ignitionConfirmation)
		xbee_transmit_char(xbeeIgnition, "DSTA");
}
