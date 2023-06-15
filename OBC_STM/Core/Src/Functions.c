#include "Functions.h"
#include "iwdg.h"

// Private functions declarations:
void sr_clk_reset(GPIO_TypeDef *cotsClkPort, uint16_t cotsClkPin);
void sr_clr_set(GPIO_TypeDef *cotsClrPort, uint16_t cotsClrPin);

// Global variables:
DataFromComm dataFromComm;
uint8_t rxFlag;
RecoveryData recData;
__IO uint32_t TimingDelay;

# define ADC_LEN 2
uint16_t adc_tab[ADC_LEN];

void initAll(void) {

	memset(&recData, 0, sizeof(RecoveryData));
	HAL_I2C_EnableListen_IT(&hi2c1);
	HAL_ADC_Start_DMA(&hadc1, (uint32_t*) adc_tab, ADC_LEN);

	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
	HAL_Delay(50);
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
}

/*****************************************************************/

void checkCOTS(void) {

	recData.easyMiniFirstStage 		|= !HAL_GPIO_ReadPin(MiniDrogueCheck_GPIO_Port, MiniDrogueCheck_Pin);
	recData.easyMiniSecondStage 	|= !HAL_GPIO_ReadPin(MiniBigCheck_GPIO_Port, MiniBigCheck_Pin);

	recData.telemetrumFirstStage 	|= !HAL_GPIO_ReadPin(TeleDrogueCheck_GPIO_Port, TeleDrogueCheck_Pin);
	recData.telemetrumSecondStage 	|= !HAL_GPIO_ReadPin(TeleBigCheck_GPIO_Port, TeleBigCheck_Pin);

	// Check if the second recovery happened:
	if (recData.firstStageDone && !recData.secondStageDone &&
		(recData.telemetrumSecondStage || recData.easyMiniSecondStage)) {

		doSecondSeparation();
	}
}

/*****************************************************************/

void checkParameters(void) {

	// COTS:
	checkCOTS();

	// Nose cone breaking wire continuity:
	recData.separationSwitch1 		= !HAL_GPIO_ReadPin(EndStop1_GPIO_Port, EndStop1_Pin);
	recData.separationSwitch2 		= !HAL_GPIO_ReadPin(EndStop2_GPIO_Port, EndStop2_Pin);

	// Nie używane w rakiecie R5 Aurora:
	// recData.firstStageContinouity 	= !(Igni1Cont_GPIO_Port->IDR & Igni1Cont_Pin);

	// EasyMini igniter continuity and pressure from both valves:
	recData.secondStageContinouity 	= !HAL_GPIO_ReadPin(Igni2Cont_GPIO_Port, Igni2Cont_Pin);
	recData.pressure1 = adc_tab[0];
	recData.pressure2 = adc_tab[1];

	// Attiny (if one runs valve, the other will run too):
	if (HAL_GPIO_ReadPin(AttTest1_GPIO_Port, AttTest1_Pin))		 doServoSeparation();
	else if (HAL_GPIO_ReadPin(AttTest2_GPIO_Port, AttTest2_Pin)) doServoSeparation();
}

/*****************************************************************/

void doServoSeparation(void) {

	// Only servos, no pyro!
	HAL_GPIO_WritePin(ForceServo1_GPIO_Port, ForceServo1_Pin, 1);
	HAL_GPIO_WritePin(ForceServo2_GPIO_Port, ForceServo2_Pin, 1);
	recData.firstStageDone = 1;
}

/*****************************************************************/

void doSecondSeparation(void) {

	HAL_GPIO_WritePin(Igni2Fire_GPIO_Port, Igni2Fire_Pin, 1);

	for (uint16_t i = 0; i < 10; i++) {

		HAL_Delay(100);

		// Watchdog refresh:
    	HAL_IWDG_Refresh(&hiwdg);
	}

	HAL_GPIO_WritePin(ForceServo1_GPIO_Port, ForceServo1_Pin, 0);
	recData.secondStageDone = 1;
}

/*****************************************************************/

void executeCommand(DataFromComm _dataFromComm) {

	switch (_dataFromComm.command) {

	case 1:		easyArm();  			break;
	case 2: 	disarm(); 				break;
	case 3:		teleArm();				break;
	case 4: 	disarm(); 				break;
	case 165: 	doServoSeparation(); 	break;
	case 90: 	doSecondSeparation(); 	break;
	}
}

/*****************************************************************/

void easyArm() {

	sr_clr_set(SR_EasyArm_GPIO_Port, SR_EasyArm_Pin);
	sr_clk_reset(SR_EasyClk_GPIO_Port, SR_EasyClk_Pin);

	recData.isArmed = 1;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

/*****************************************************************/

void disarm() {

	// Disarming both COTS at the same time (common SR_CLR_Pin):
	HAL_GPIO_WritePin(SR_CLR_GPIO_Port, SR_CLR_Pin, 0);
	HAL_GPIO_WritePin(SR_RCLK_GPIO_Port, SR_RCLK_Pin, 1);
	HAL_GPIO_WritePin(SR_EasyArm_GPIO_Port, SR_EasyArm_Pin, 0);
	HAL_GPIO_WritePin(SR_TeleArm_GPIO_Port, SR_TeleArm_Pin, 0);
	HAL_GPIO_WritePin(SR_RCLK_GPIO_Port, SR_RCLK_Pin, 0);

	recData.isArmed = 0;
	recData.isTeleActive = 0;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 0);
}

/*****************************************************************/

void teleArm() {

	sr_clr_set(SR_TeleArm_GPIO_Port, SR_TeleArm_Pin);
	sr_clk_reset(SR_TeleClk_GPIO_Port, SR_TeleClk_Pin);

	recData.isTeleActive = 1;
	HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, 1);
}

/****************************************************************
* SR functions:
****************************************************************/

void sr_clk_reset(GPIO_TypeDef *cotsClkPort, uint16_t cotsClkPin) {

	HAL_GPIO_WritePin(cotsClkPort, cotsClkPin,1);
	HAL_Delay(2);
	HAL_GPIO_WritePin(cotsClkPort, cotsClkPin, 0);
	HAL_Delay(2);
	HAL_GPIO_WritePin(SR_RCLK_GPIO_Port, SR_RCLK_Pin,1);
	HAL_Delay(2);
	HAL_GPIO_WritePin(SR_RCLK_GPIO_Port, SR_RCLK_Pin, 0);
}

/*****************************************************************/

void sr_clr_set(GPIO_TypeDef *cotsClrPort, uint16_t cotsClrPin) {

	HAL_GPIO_WritePin(SR_CLR_GPIO_Port, SR_CLR_Pin, 1);
	HAL_Delay(2);
	HAL_GPIO_WritePin(cotsClrPort, cotsClrPin, 1);
	HAL_Delay(2);
}
