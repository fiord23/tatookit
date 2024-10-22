/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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
#define POWER_ON_OFF_Pin GPIO_PIN_0
#define POWER_ON_OFF_GPIO_Port GPIOA
#define POWER_PWM_MODE_Pin GPIO_PIN_1
#define POWER_PWM_MODE_GPIO_Port GPIOA
#define EN_IN1_Pin GPIO_PIN_2
#define EN_IN1_GPIO_Port GPIOA
#define PH_IN2_Pin GPIO_PIN_3
#define PH_IN2_GPIO_Port GPIOA
#define PB_CLR_Pin GPIO_PIN_4
#define PB_CLR_GPIO_Port GPIOA
#define BUTTON_POWER_Pin GPIO_PIN_5
#define BUTTON_POWER_GPIO_Port GPIOA
#define IPROPI_Pin GPIO_PIN_6
#define IPROPI_GPIO_Port GPIOA
#define POWER_ADC_Pin GPIO_PIN_0
#define POWER_ADC_GPIO_Port GPIOB
#define BUTTON_1_Pin GPIO_PIN_1
#define BUTTON_1_GPIO_Port GPIOB
#define POWER_VOLTAGE_REMOTE_Pin GPIO_PIN_8
#define POWER_VOLTAGE_REMOTE_GPIO_Port GPIOA
#define DISPLAY_CS_Pin GPIO_PIN_9
#define DISPLAY_CS_GPIO_Port GPIOA
#define DISPLAY_DC_Pin GPIO_PIN_10
#define DISPLAY_DC_GPIO_Port GPIOA
#define PB_INT_Pin GPIO_PIN_11
#define PB_INT_GPIO_Port GPIOA
#define NFAULT_Pin GPIO_PIN_12
#define NFAULT_GPIO_Port GPIOA
#define RCOUT_Pin GPIO_PIN_15
#define RCOUT_GPIO_Port GPIOA
#define BUTTON_2_Pin GPIO_PIN_6
#define BUTTON_2_GPIO_Port GPIOB
#define NSLEEP_Pin GPIO_PIN_7
#define NSLEEP_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
