#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include <string.h>
#include <stdbool.h>
#include "gpio.h"
#include "i2c.h"

typedef struct {

	uint8_t command;
	uint16_t commandArg;

} DataFromComm;

DataFromComm dataFromComm;

uint8_t rxFlag;

typedef struct {

	bool isArmed :1;
	bool firstStageContinouity :1;
	bool secondStageContinouity :1;
	bool separationSwitch1 :1;
	bool separationSwitch2 :1;
	bool telemetrumFirstStage :1;
	bool telemetrumSecondStage :1;
	bool altimaxFirstStage :1;
	bool altimaxSecondStage :1;
	bool apogemixFirstStage :1;
	bool apogemixSecondStage :1;
	bool firstStageDone :1;
	bool secondStageDone :1;
	bool isTeleActive :1;

} RecoveryData;

RecoveryData recData;

__IO uint32_t TimingDelay;

void initAll(void);


void checkComputers(void);

void doFirstSeparation(void);

void doSecondSeparation(void);

void executeCommand(DataFromComm dataFromComm);

void armDisarm(bool on);

void teleOnOff(bool on);

#endif /* INC_FUNCTIONS_H_ */
