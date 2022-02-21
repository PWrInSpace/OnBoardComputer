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

	uint32_t time_ms;
	float batteryV;
	float tankPressure;
	float gpsLatitude;
	float gpsLongitude;
	float gpsAltitude;
	uint8_t gpsSatNum;
	uint8_t gpsTimeS;
	uint16_t hallSensors[5];

	float tanWaVoltage;
	uint8_t tanWaState : 4;
	_Bool tanWaIgniter : 1;
	uint8_t tanWaFill : 2;
	uint8_t tanWaDepr : 2;
	uint8_t tanWaUpust : 2;
	float rocketWeightKg;
	float tankWeightKg;
	uint32_t rocketWRaw;
	uint32_t tankWRaw;

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

void loraReaction(void);

void generateAndSendFrame(void);

void updateFrame(void);

#endif /* INC_FUNKCJE_H_ */
