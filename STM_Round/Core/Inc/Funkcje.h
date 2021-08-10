#ifndef INC_FUNKCJE_H_
#define INC_FUNKCJE_H_

#include "LoraRFM.h"
#include "GPS.h"
#include <string.h>
#include "xbee.h"

/******************************/

#define RX_BUFFER_SIZE 128
#define GPS_PERIOD 5000

typedef struct {

	uint32_t gpsFrameTimer;

	_Bool tanwaRxFlag;
	_Bool maincompRxFlag;

	char tanwaString[RX_BUFFER_SIZE];
	char maincompString[RX_BUFFER_SIZE];
	char bufferLoraTx[BUFFER_SIZE];

} TimersFlagsStrings;

/******************************/

TimersFlagsStrings timersFlagsStrings;

Xbee xbeeIgnition;

/******************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void initAll(void);

void logAndSendDataLoop(void);

void logDataLoop(void);

void loraReaction(void);

#endif /* INC_FUNKCJE_H_ */
