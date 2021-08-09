#ifndef INC_FUNKCJE_H_
#define INC_FUNKCJE_H_

#include "LoraRFM.h"
#include "GPS.h"
#include <string.h>
#include "xbee.h"

/******************************/

typedef struct {

	uint32_t sendDataTimer;
	uint32_t logDataTimer;
	uint16_t sendDataPeriod;
	uint16_t logDataPeriod;

	uint32_t tenSecondTimer;
	int launchTimer;
	int checkConnectionTimer;

} Timers;

/******************************/

char bufferLoraTx[BUFFER_SIZE];
char separationBufferRx[10];

Timers timers;

Xbee xbeePrandl;
Xbee xbeeIgnition;

/******************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void initAll(void);

void logAndSendDataLoop(void);

void logDataLoop(void);

void setPeriods(void);

void loraReaction(void);

#endif /* INC_FUNKCJE_H_ */
