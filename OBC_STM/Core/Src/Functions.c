#include "Functions.h"

// Global variables:
DataFromComm dataFromComm;
uint8_t rxFlag;
RecoveryData recData;
__IO uint32_t TimingDelay;

void initAll(void) {

	memset(&recData, 0, sizeof(RecoveryData));
	HAL_I2C_EnableListen_IT(&hi2c1);
}

/*****************************************************************/

void checkCOTS(void) {

	if (recData.isArmed) {
		recData.easyMiniFirstStage 		= MiniDrogueCheck_GPIO_Port->IDR & MiniDrogueCheck_Pin;
		recData.easyMiniSecondStage 	= MiniBigCheck_GPIO_Port->IDR & MiniBigCheck_Pin;

		recData.telemetrumFirstStage 	= TeleDrogueCheck_GPIO_Port->IDR & TeleDrogueCheck_Pin;
		recData.telemetrumSecondStage 	= TeleBigCheck_GPIO_Port->IDR & TeleBigCheck_Pin;
	}
}

/*****************************************************************/

void checkComputers(void) {

	// COTSy:
	checkCOTS();

	// Krańcówki + ciągłość + ciśnienie:
	recData.separationSwitch1 		= !(EndStop1_GPIO_Port->IDR & EndStop1_Pin);
	recData.separationSwitch2 		= !(EndStop2_GPIO_Port->IDR & EndStop2_Pin);
	recData.firstStageContinouity 	= !(Igni1Cont_GPIO_Port->IDR & Igni1Cont_Pin);
	recData.secondStageContinouity 	= !(Igni2Cont_GPIO_Port->IDR & Igni2Cont_Pin);
	recData.pressure1 = 2137; // TODO!!!
	recData.pressure2 = 2137; // TODO!!!
}

/*****************************************************************/

void doFirstSeparation(void) {

	Igni1Fire_GPIO_Port->ODR |= Igni1Fire_Pin;
	ForceServo1_GPIO_Port->ODR |= ForceServo1_Pin;
	ForceServo2_GPIO_Port->ODR |= ForceServo2_Pin;

	for (uint16_t i = 0; i < 100; i++) {

		checkComputers();
		HAL_Delay(20);
	}

	Igni1Fire_GPIO_Port->ODR &= ~Igni1Fire_Pin;
	recData.firstStageDone = 1;
}

/*****************************************************************/

void doSecondSeparation(void) {

	Igni2Fire_GPIO_Port->ODR |= Igni2Fire_Pin;

	for (uint16_t i = 0; i < 100; i++) {

		checkComputers();
		HAL_Delay(20);
	}

	Igni2Fire_GPIO_Port->ODR &= ~Igni2Fire_Pin;
	recData.secondStageDone = 1;
}

/*****************************************************************/

void executeCommand(DataFromComm _dataFromComm) {

	switch (_dataFromComm.command) {

	case 1:		armDisarm(1);  			break;
	case 2: 	armDisarm(0); 			break;
	case 3:		teleOnOff(1);			break;
	case 4: 	teleOnOff(0); 			break;
	case 165: 	doFirstSeparation(); 	break;
	case 90: 	doSecondSeparation(); 	break;
	}
}

/*****************************************************************/

void armDisarm(bool on) {

	if (on) {
		SoftArm_GPIO_Port->ODR |= SoftArm_Pin;
		HAL_Delay(10);
		recData.isArmed = 1;
	}
	else {

		SoftArm_GPIO_Port->ODR &= ~SoftArm_Pin;
		recData.isArmed = 0;
	}
}

/*****************************************************************/

void teleOnOff(bool on) {

	if (on) {
		TeleArm_GPIO_Port->ODR |= TeleArm_Pin;
		HAL_Delay(10);
		recData.isTeleActive = 1;
	}
	else {

		TeleArm_GPIO_Port->ODR &= ~TeleArm_Pin;
		recData.isTeleActive = 0;
	}
}
