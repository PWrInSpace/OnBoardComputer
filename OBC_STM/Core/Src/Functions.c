#include "Functions.h"
#include "iwdg.h"

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

	LED_GPIO_Port->ODR |= LED_Pin;
	HAL_Delay(100);
	LED_GPIO_Port->ODR &= ~LED_Pin;
}

/*****************************************************************/

void checkCOTS(void) {

	if (recData.isArmed) {
		recData.easyMiniFirstStage 		= !(MiniDrogueCheck_GPIO_Port->IDR & MiniDrogueCheck_Pin);
		recData.easyMiniSecondStage 	= !(MiniBigCheck_GPIO_Port->IDR & MiniBigCheck_Pin);

		recData.telemetrumFirstStage 	= !(TeleDrogueCheck_GPIO_Port->IDR & TeleDrogueCheck_Pin);
		recData.telemetrumSecondStage 	= !(TeleBigCheck_GPIO_Port->IDR & TeleBigCheck_Pin);
	}
}

/*****************************************************************/

void checkParameters(void) {

	// COTSy:
	checkCOTS();

	// Krańcówki + ciągłość + ciśnienie:
	recData.separationSwitch1 		= !(EndStop1_GPIO_Port->IDR & EndStop1_Pin);
	recData.separationSwitch2 		= !(EndStop2_GPIO_Port->IDR & EndStop2_Pin);
	recData.firstStageContinouity 	= !(Igni1Cont_GPIO_Port->IDR & Igni1Cont_Pin);
	recData.secondStageContinouity 	= !(Igni2Cont_GPIO_Port->IDR & Igni2Cont_Pin);
	recData.pressure1 = adc_tab[0];
	recData.pressure2 = adc_tab[1];

	// Attiny (jeśli odpali jedno, to ma też odpalić drugie):
	if (AttTest1_GPIO_Port->IDR & AttTest1_Pin) doServoSeparation();
	else if (AttTest2_GPIO_Port->IDR & AttTest2_Pin) doServoSeparation();
}

/*****************************************************************/

void doServoSeparation(void) {

	// Only servos, no pyro!
	ForceServo1_GPIO_Port->ODR |= ForceServo1_Pin;
	ForceServo2_GPIO_Port->ODR |= ForceServo2_Pin;
	recData.firstStageDone = 1;
}

/*****************************************************************/

void doSecondSeparation(void) {

	Igni2Fire_GPIO_Port->ODR |= Igni2Fire_Pin;

	for (uint16_t i = 0; i < 100; i++) {

		checkParameters();
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
	case 165: 	doServoSeparation(); 	break;
	case 90: 	doSecondSeparation(); 	break;
	}
}

/*****************************************************************/

void armDisarm(bool on) {

	if (on) {
	  	HAL_GPIO_WritePin(SR_CLR_GPIO_Port,SR_CLR_Pin,GPIO_PIN_SET);
 	 	HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_EasyArm_GPIO_Port,SR_EasyArm_Pin,GPIO_PIN_SET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_EasyClk_GPIO_Port,SR_EasyClk_Pin,GPIO_PIN_SET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_EasyClk_GPIO_Port,SR_EasyClk_Pin,GPIO_PIN_RESET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_RESET);

		recData.isArmed = 1;
		LED_GPIO_Port->ODR |= LED_Pin ;
	}
	else {

		HAL_GPIO_WritePin(SR_CLR_GPIO_Port,SR_CLR_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_SET);
  		HAL_GPIO_WritePin(SR_EasyArm_GPIO_Port,SR_EasyArm_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_RESET);

		recData.isArmed = 0;
		LED_GPIO_Port->ODR &= ~LED_Pin;
	}
}

/*****************************************************************/

void teleOnOff(bool on) {

	if (on) {

		HAL_GPIO_WritePin(SR_CLR_GPIO_Port,SR_CLR_Pin,GPIO_PIN_SET);
 	 	HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_TeleArm_GPIO_Port,SR_TeleArm_Pin,GPIO_PIN_SET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_TeleClk_GPIO_Port,SR_TeleClk_Pin,GPIO_PIN_SET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_TeleClk_GPIO_Port,SR_TeleClk_Pin,GPIO_PIN_RESET);
  		HAL_Delay(2);
  		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_SET);
		HAL_Delay(2);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_RESET);

		recData.isTeleActive = 1;
	}
	else {

		HAL_GPIO_WritePin(SR_CLR_GPIO_Port,SR_CLR_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_SET);
		HAL_GPIO_WritePin(SR_TeleArm_GPIO_Port,SR_TeleArm_Pin,GPIO_PIN_RESET);
  		HAL_GPIO_WritePin(SR_EasyArm_GPIO_Port,SR_EasyArm_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SR_RCLK_GPIO_Port,SR_RCLK_Pin,GPIO_PIN_RESET);
		HAL_GPIO_WritePin(SR_CLR_GPIO_Port,SR_CLR_Pin,GPIO_PIN_SET);

		recData.isTeleActive = 0;
	}
}
