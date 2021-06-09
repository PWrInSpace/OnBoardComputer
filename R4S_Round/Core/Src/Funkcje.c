#include "Funkcje.h"

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == _GPS_USART.Instance)
		GPS_CallBack();

	if (huart->Instance == USART2) {
		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
		HAL_UART_DMAStop(&huart2);

		xbee_receive();
		if (xbee_rx.data_flag) { //jeżeli wiadomość była danymi to ta zmienna będzie miała wartość 1

			/*TUTAJ WEDLE UZNANIA PRZECHWYTUJECIE DANE KTORE PRZYSZŁY
			 macie do dyspozycji tablice 'xbee_rx.data_array' o wielkości 'DATA_ARRAY' - 30, w której są wartości
			 jeżeli chcecie zatrzymać te dane musicie skopiować wartości tej tabilicy
			 pobranie adresu jest złym pomysłem bo przy każdym odebraniu tablica zmienia swoją zawartosć*/

		}

		HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded,
		DATA_LENGTH);
	}
}

/*******************************************************************************************/

void initAll(void) {

	rocketState = INIT;
	loraInit();
	GPS_Init();

	__HAL_UART_ENABLE_IT(&huart2, UART_IT_IDLE);
	HAL_UART_Receive_DMA(&huart2, (uint8_t*) xbee_rx.mess_loaded, DATA_LENGTH);
	xbee_init(&xbeePrandl, 0x0013A20041C283D6, &huart2);
	xbee_init(&xbeeIgnition, 0x0013A20041C283D6, &huart2);

	sd_spi_init();
	f_open(&file, "plik.txt", FA_WRITE | FA_OPEN_APPEND);
	f_close(&file);

	timers.logDataTimer = uwTick;
	timers.sendDataTimer = uwTick;
}

/*******************************************************************************************/

void logAndSendDataLoop(void) {

	GPS_Process();

	otherData.sdState = !otherData.sdState;

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

	f_open(&file, "plik.txt", FA_WRITE | FA_OPEN_APPEND);
	fres = f_write(&file, (BYTE*) bufferLoraTx, strlen(bufferLoraTx),
			&bytesWrote);

	/*if (fres != 0)
	 otherData.sdState = 0;
	 else
	 otherData.sdState = 1;*/

	f_close(&file);
}

/*******************************************************************************************/

void setPeriods(void) {

	switch (rocketState) {

	case INIT:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;

	case IDLE:
		break;
	case ARMED_HARD:
		break;

	case ARMED_SOFT:
		break;

	case READY:

		timers.sendDataPeriod = 2000;
		break;

	case FLIGHT:

		timers.sendDataPeriod = 1000;
		timers.logDataPeriod = 50;
		break;

	case ABORT:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 0;
		break;

	case FIRST_SEPAR:

		timers.sendDataPeriod = 2000;
		timers.logDataPeriod = 200;
		break;

	case SECOND_SEPAR:

		timers.sendDataPeriod = 5000;
		timers.logDataPeriod = 500;
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

		if ((int)rocketState == loraBuffer[5] - '0')
			rocketState = loraBuffer[7] - '0';
	}

	memset(loraBuffer, 0, BUFFER_SIZE);
}
