#include "Functions.h"

void initAll(void) {

	memset(&recData, 0, sizeof(RecoveryData));
	HAL_I2C_EnableListen_IT(&hi2c1);
}

/*****************************************************************/

#define TEST_NUM 2

void checkCOTS(void) {

	bool altiTestPilot[TEST_NUM];
	bool teleTestPilot[TEST_NUM];
	bool altiTestBig[TEST_NUM];
	bool teleTestBig[TEST_NUM];

	for (uint8_t i = 0; i < TEST_NUM; i++) {

		altiTestPilot[i] 	= AltiPilotCheck_GPIO_Port->IDR & AltiPilotCheck_Pin;
		altiTestBig[i] 		= AltiDuzyCheck_GPIO_Port->IDR & AltiDuzyCheck_Pin;
		teleTestPilot[i]	= TelPilotCheck_GPIO_Port->IDR & TelPilotCheck_Pin;
		teleTestBig[i]		= TelDuzyCheck_GPIO_Port->IDR & TelDuzyCheck_Pin;
		HAL_Delay(10);
	}

	if (recData.isArmed) {
		recData.altimaxFirstStage 		= altiTestPilot[0] 	&& altiTestPilot [1];
		recData.altimaxSecondStage 		= altiTestBig[0] 	&& altiTestBig[1];

		if (recData.isTeleActive) {
			recData.telemetrumFirstStage 	= teleTestPilot[0]	&& teleTestPilot[1];
			recData.telemetrumSecondStage 	= teleTestBig[0]	&& teleTestBig[1];
		}
	}
}

/*****************************************************************/

void checkComputers(void) {

	// COTSy + Apogemix:
	checkCOTS();
	recData.apogemixFirstStage 		= !(ApogPilotCheck_GPIO_Port->IDR & ApogPilotCheck_Pin);
	recData.apogemixSecondStage 	= !(ApogDuzyCheck_GPIO_Port->IDR & ApogDuzyCheck_Pin);

	// Krańcówki + ciągłość:
	recData.separationSwitch1 		= !(SepaSw1_GPIO_Port->IDR & SepaSw1_Pin);
	recData.separationSwitch2 		= !(SepaSw2_GPIO_Port->IDR & SepaSw2_Pin);
	recData.firstStageContinouity 	= !(Igniter1Cont_GPIO_Port->IDR & Igniter1Cont_Pin);
	recData.secondStageContinouity 	= !(Igniter2Cont_GPIO_Port->IDR & Igniter2Cont_Pin);

	// Arming Check:
	recData.isArmed = SoftArm_GPIO_Port->IDR & SoftArm_Pin;

	// Telemetrum Arming Check:
	recData.isTeleActive = TelArm_GPIO_Port->IDR & TelArm_Pin;
}

/*****************************************************************/

void doFirstSeparation(void) {

	_Bool workingSwitch = recData.separationSwitch1;
	Igniter1Fire_GPIO_Port->ODR |= Igniter1Fire_Pin;

	for (uint16_t i = 0; i < 200; i++) {

		checkComputers(); // Trwa 10ms

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

	for (uint16_t i = 0; i < 200; i++) {

		checkComputers(); // Trwa 10ms

		// Jeśli wcześniej była ciągłość głowicy, a teraz jej nie ma, to znaczy, że separacja się udała i można wyłączyć mosfet:
		if (!recData.separationSwitch2 && workingSwitch) break;
	}

	Igniter2Fire_GPIO_Port->ODR &= ~Igniter2Fire_Pin;
	recData.secondStageDone = 1;
}

/*****************************************************************/

void executeCommand(DataFromComm _dataFromComm) {

	switch (_dataFromComm.command) {

	case 1:		SoftArm_GPIO_Port->ODR |= SoftArm_Pin;  break;
	case 2: 	SoftArm_GPIO_Port->ODR &= ~SoftArm_Pin; break;
	case 3:		TelArm_GPIO_Port->ODR |= TelArm_Pin;	break;
	case 4: 	TelArm_GPIO_Port->ODR &= ~TelArm_Pin; 	break;
	case 165: 	doFirstSeparation(); 					break;
	case 90: 	doSecondSeparation(); 					break;
	}

	// TODO Silniki na Portugalię :)
}
