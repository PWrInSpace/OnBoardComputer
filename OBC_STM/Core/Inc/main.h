/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SR_OE_Pin GPIO_PIN_13
#define SR_OE_GPIO_Port GPIOC
#define SR_CLR_Pin GPIO_PIN_14
#define SR_CLR_GPIO_Port GPIOC
#define SR_RCLK_Pin GPIO_PIN_15
#define SR_RCLK_GPIO_Port GPIOC
#define SR_TeleArm_Pin GPIO_PIN_0
#define SR_TeleArm_GPIO_Port GPIOD
#define SR_TeleClk_Pin GPIO_PIN_1
#define SR_TeleClk_GPIO_Port GPIOD
#define LED_Pin GPIO_PIN_0
#define LED_GPIO_Port GPIOA
#define ForceServo1_Pin GPIO_PIN_1
#define ForceServo1_GPIO_Port GPIOA
#define ForceServo2_Pin GPIO_PIN_2
#define ForceServo2_GPIO_Port GPIOA
#define Igni1Cont_Pin GPIO_PIN_3
#define Igni1Cont_GPIO_Port GPIOA
#define Igni2Cont_Pin GPIO_PIN_4
#define Igni2Cont_GPIO_Port GPIOA
#define Igni1Fire_Pin GPIO_PIN_5
#define Igni1Fire_GPIO_Port GPIOA
#define Igni2Fire_Pin GPIO_PIN_6
#define Igni2Fire_GPIO_Port GPIOA
#define Pressure1_Pin GPIO_PIN_0
#define Pressure1_GPIO_Port GPIOB
#define Pressure2_Pin GPIO_PIN_1
#define Pressure2_GPIO_Port GPIOB
#define Mos1_Pin GPIO_PIN_13
#define Mos1_GPIO_Port GPIOB
#define Mos2_Pin GPIO_PIN_14
#define Mos2_GPIO_Port GPIOB
#define TeleBigCheck_Pin GPIO_PIN_9
#define TeleBigCheck_GPIO_Port GPIOA
#define TeleDrogueCheck_Pin GPIO_PIN_10
#define TeleDrogueCheck_GPIO_Port GPIOA
#define MiniBigCheck_Pin GPIO_PIN_11
#define MiniBigCheck_GPIO_Port GPIOA
#define MiniDrogueCheck_Pin GPIO_PIN_12
#define MiniDrogueCheck_GPIO_Port GPIOA
#define SR_EasyArm_Pin GPIO_PIN_15
#define SR_EasyArm_GPIO_Port GPIOA
#define EndStop1_Pin GPIO_PIN_3
#define EndStop1_GPIO_Port GPIOB
#define EndStop2_Pin GPIO_PIN_4
#define EndStop2_GPIO_Port GPIOB
#define AttTest1_Pin GPIO_PIN_5
#define AttTest1_GPIO_Port GPIOB
#define AttTest2_Pin GPIO_PIN_6
#define AttTest2_GPIO_Port GPIOB
#define SR_EasyClk_Pin GPIO_PIN_7
#define SR_EasyClk_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
