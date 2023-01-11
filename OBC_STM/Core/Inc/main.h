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
#define TelArm_Pin GPIO_PIN_13
#define TelArm_GPIO_Port GPIOC
#define Dir2_1_Pin GPIO_PIN_0
#define Dir2_1_GPIO_Port GPIOA
#define Dir2_2_Pin GPIO_PIN_1
#define Dir2_2_GPIO_Port GPIOA
#define Dir3_2_Pin GPIO_PIN_2
#define Dir3_2_GPIO_Port GPIOA
#define Dir3_1_Pin GPIO_PIN_3
#define Dir3_1_GPIO_Port GPIOA
#define Dir4_1_Pin GPIO_PIN_4
#define Dir4_1_GPIO_Port GPIOA
#define Dir4_2_Pin GPIO_PIN_5
#define Dir4_2_GPIO_Port GPIOA
#define Igniter1Fire_Pin GPIO_PIN_6
#define Igniter1Fire_GPIO_Port GPIOA
#define Igniter2Fire_Pin GPIO_PIN_7
#define Igniter2Fire_GPIO_Port GPIOA
#define Igniter1Cont_Pin GPIO_PIN_0
#define Igniter1Cont_GPIO_Port GPIOB
#define Igniter2Cont_Pin GPIO_PIN_1
#define Igniter2Cont_GPIO_Port GPIOB
#define ApogDuzyCheck_Pin GPIO_PIN_12
#define ApogDuzyCheck_GPIO_Port GPIOB
#define ApogPilotCheck_Pin GPIO_PIN_13
#define ApogPilotCheck_GPIO_Port GPIOB
#define SoftArm_Pin GPIO_PIN_14
#define SoftArm_GPIO_Port GPIOB
#define SepaSw1_Pin GPIO_PIN_15
#define SepaSw1_GPIO_Port GPIOB
#define SepaSw2_Pin GPIO_PIN_8
#define SepaSw2_GPIO_Port GPIOA
#define AltiPilotCheck_Pin GPIO_PIN_4
#define AltiPilotCheck_GPIO_Port GPIOB
#define AltiDuzyCheck_Pin GPIO_PIN_5
#define AltiDuzyCheck_GPIO_Port GPIOB
#define TelPilotCheck_Pin GPIO_PIN_6
#define TelPilotCheck_GPIO_Port GPIOB
#define TelDuzyCheck_Pin GPIO_PIN_7
#define TelDuzyCheck_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
