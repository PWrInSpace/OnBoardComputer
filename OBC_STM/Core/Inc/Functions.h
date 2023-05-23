#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include <string.h>
#include <stdbool.h>
#include "gpio.h"
#include "i2c.h"
#include "adc.h"

typedef struct {

	uint32_t command;
	int32_t commandArg;

} DataFromComm;

typedef struct {

	bool isArmed :1;
	bool isTeleActive :1;
	bool easyMiniFirstStage :1;
	bool easyMiniSecondStage :1;
	bool telemetrumFirstStage :1;
	bool telemetrumSecondStage :1;
	bool firstStageDone :1;
	bool secondStageDone :1;
	bool firstStageContinouity :1;
	bool secondStageContinouity :1;
	bool separationSwitch1 :1;
	bool separationSwitch2 :1;

	uint16_t pressure1;
	uint16_t pressure2;

} RecoveryData;

// Externs:
extern DataFromComm dataFromComm;
extern uint8_t rxFlag;
extern RecoveryData recData;
extern __IO uint32_t TimingDelay;

// Functions:
void initAll(void);

void checkParameters(void);

void doServoSeparation(void);

void doSecondSeparation(void);

void executeCommand(DataFromComm dataFromComm);

void armDisarm(bool on);

void teleOnOff(bool on);

#endif /* INC_FUNCTIONS_H_ */
