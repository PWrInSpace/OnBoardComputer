#ifndef INC_FUNKCJE_H_
#define INC_FUNKCJE_H_

#include "LoraRFM.h"
#include "GPS.h"
#include <string.h>
#include "xbee.h"
#include "adc.h"

/******************************/

#define RX_BUFFER_SIZE 128

typedef struct {

	uint32_t gpsFrameTimer;

	_Bool tanwaRxFlag;
	_Bool maincompRxFlag;

	char gpsStringLora[RX_BUFFER_SIZE];
	char maincompStringDma[RX_BUFFER_SIZE];
	char tanwaStringLora[RX_BUFFER_SIZE];
	char maincompStringLora[RX_BUFFER_SIZE];

} TimersFlagsStrings;

/******************************/

TimersFlagsStrings tfsStruct;

Xbee xbeeIgnition;

uint16_t hallSensors[5];
uint16_t badFrames;

uint16_t gpsPeriod;

_Bool ignite;

/******************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void initAll(void);

void sendGPSData(void);

void sendFromMaincompToLora(void);

void sendFromTanwaToLora(void);

void loraReaction(void);

#endif /* INC_FUNKCJE_H_ */
