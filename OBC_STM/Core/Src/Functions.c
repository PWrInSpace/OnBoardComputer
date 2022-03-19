#include "Functions.h"

void initAll(void) {

	memset(&recData, 0, sizeof(RecoveryData));
	HAL_I2C_EnableListen_IT(&hi2c2);
}

/*****************************************************************/

void checkComputers(void) {

	// COTSy + Apogemix:
	recData.altimaxFirstStage 		= !(AltiPilotCheck_GPIO_Port->IDR & AltiPilotCheck_Pin);
	recData.altimaxSecondStage 		= !(AltiDuzyCheck_GPIO_Port->IDR & AltiDuzyCheck_Pin);
	recData.telemetrumFirstStage 	= !(TelPilotCheck_GPIO_Port->IDR & TelPilotCheck_Pin);
	recData.telemetrumSecondStage 	= !(TelDuzyCheck_GPIO_Port->IDR & TelDuzyCheck_Pin);
	recData.apogemixFirstStage 		= !(ApogPilotCheck_GPIO_Port->IDR & ApogPilotCheck_Pin);
	recData.apogemixSecondStage 	= !(ApogDuzyCheck_GPIO_Port->IDR & ApogDuzyCheck_Pin);

	// Krańcówki + ciągłość:
	recData.separationSwitch1 		= !(SepaSw1_GPIO_Port->IDR & SepaSw1_Pin);
	recData.separationSwitch2 		= !(SepaSw2_GPIO_Port->IDR & SepaSw2_Pin);
	recData.firstStageContinouity 	= !(Igniter1Cont_GPIO_Port->IDR & Igniter1Cont_Pin);
	recData.secondStageContinouity 	= !(Igniter2Cont_GPIO_Port->IDR & Igniter2Cont_Pin);
}

/*****************************************************************/

void armDisarm(_Bool arm) {

	if (arm) {
		TelArm_GPIO_Port->ODR |= TelArm_Pin;
		SoftArm_GPIO_Port->ODR |= SoftArm_Pin;
	} else {
		TelArm_GPIO_Port->ODR &= ~TelArm_Pin;
		SoftArm_GPIO_Port->ODR &= ~SoftArm_Pin;
	}
}

/*****************************************************************/

void doFirstSeparation(void) {

	_Bool workingSwitch = recData.separationSwitch1;
	Igniter1Fire_GPIO_Port->ODR |= Igniter1Fire_Pin;

	for (uint16_t i = 0; i < 2000; i++) {

		checkComputers();
		HAL_Delay(1);

		// Jeśli wcześniej była ciągłość głowicy, a teraz jej nie ma, to znaczy, że separacja się udała i można wyłączyć mosfet:
		if (!recData.separationSwitch1 && workingSwitch) break;
	}

	Igniter1Fire_GPIO_Port->ODR &= ~Igniter1Fire_Pin;
	recData.firstStageDone = 1;
}

/*****************************************************************/

void doSecondSeparation(void) {

	_Bool workingSwitch = recData.separationSwitch2;
	Igniter2Fire_GPIO_Port->ODR |= Igniter2Fire_Pin;

	for (uint16_t i = 0; i < 2000; i++) {

		checkComputers();
		HAL_Delay(1);

		// Jeśli wcześniej była ciągłość głowicy, a teraz jej nie ma, to znaczy, że separacja się udała i można wyłączyć mosfet:
		if (!recData.separationSwitch2 && workingSwitch) break;
	}

	Igniter2Fire_GPIO_Port->ODR &= ~Igniter2Fire_Pin;
	recData.secondStageDone = 1;
}

/*****************************************************************/

void executeCommand(uint8_t commandNumber) {

	// TODO!!!
}
