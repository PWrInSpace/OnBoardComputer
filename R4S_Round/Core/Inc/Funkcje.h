#ifndef INC_FUNKCJE_H_
#define INC_FUNKCJE_H_

#include "LoraRFM.h"
#include "GPS.h"
#include <string.h>
#include "xbee.h"
#include "user_diskio_spi.h"

/******************************/

enum StateMachine {
	INIT = 1,
	IDLE = 2,
	ARMED_HARD = 3,
	ARMED_SOFT = 4,
	READY = 5,
	FLIGHT = 6,
	ABORT = 7,
	FIRST_SEPAR = 8,
	SECOND_SEPAR = 9,
	END = 10
} rocketState;

/******************************/

typedef struct {

	_Bool sdState;
	float pitotStatic;
	float pitotDynamic;
	int computedAltitude;
	int computedSpeed;
	_Bool separationTest1;
	_Bool separationTest2;

} OtherDataToSend;

/******************************/

typedef struct {

	uint32_t sendDataTimer;
	uint32_t logDataTimer;
	uint16_t sendDataPeriod;
	uint16_t logDataPeriod;

	uint32_t tenSecondTimer;
	int launchTimer;

} Timers;

/******************************/

// SD:
FIL file;
FRESULT fres;
UINT bytesWrote;
uint64_t allWrittenBytes;

char bufferLoraTx[BUFFER_SIZE];
char separationBufferRx[10];

OtherDataToSend otherData;
Timers timers;
_Bool ignitionConfirmation;

Xbee xbeePrandl;
Xbee xbeeIgnition;

/******************************/

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);

void initAll(void);

void testPrandl(void);

void logAndSendDataLoop(void);

void logDataLoop(void);

void setPeriods(void);

void loraReaction(void);

void doLaunch(void);

#endif /* INC_FUNKCJE_H_ */
