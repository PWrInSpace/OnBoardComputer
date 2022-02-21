#ifndef INC_FUNKCJE_H_
#define INC_FUNKCJE_H_

#include "LoraRFM.h"
#include "GPS.h"
#include <string.h>
#include "xbee.h"
#include "adc.h"

/******************************/

#define RX_BUFFER_SIZE 128

typedef struct { // TODO uzupełnić strukturę

	uint16_t hallSensors[5];

} DataFrame;

typedef struct {

	float batteryV;
	float tankPressure;

} EspBinData;

typedef struct {

	uint32_t frameTimer;
	uint32_t saveTimer;

	_Bool launched;

	EspBinData espBinData;

	char gpsStringLora[RX_BUFFER_SIZE];
	char tanwaStringLora[RX_BUFFER_SIZE];
	char txStringLora[RX_BUFFER_SIZE];

} TimersFlagsStrings;

/******************************/

#define FRAME_PERIOD 500
#define SAVE_PERIOD 100

TimersFlagsStrings tfsStruct;
DataFrame dataFrame;

Xbee xbeeIgnition;

_Bool ignite;

/******************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void initAll(void);

/*void sendGPSData(void);

void sendFromMaincompToLora(void);

void sendFromTanwaToLora(void);*/

void loraReaction(void);

void generateAndSendFrame(void);

void saveFrame(void);

#endif /* INC_FUNKCJE_H_ */
