#ifndef INC_FUNCTIONS_H_
#define INC_FUNCTIONS_H_

#include <string.h>
#include "gpio.h"
#include "i2c.h"

typedef struct {

	_Bool isArmed :1;
	_Bool firstStageContinouity :1;
	_Bool secondStageContinouity :1;
	_Bool separationSwitch1 :1;
	_Bool separationSwitch2 :1;
	_Bool telemetrumFirstStage :1;
	_Bool telemetrumSecondStage :1;
	_Bool altimaxFirstStage :1;
	_Bool altimaxSecondStage :1;
	_Bool apogemixFirstStage :1;
	_Bool apogemixSecondStage :1;
	_Bool firstStageDone :1;
	_Bool secondStageDone :1;

} RecoveryData;

RecoveryData recData;

void initAll(void);

void checkComputers(void);

void armDisarm(_Bool arm);

void doFirstSeparation(void);

void doSecondSeparation(void);

#endif /* INC_FUNCTIONS_H_ */
