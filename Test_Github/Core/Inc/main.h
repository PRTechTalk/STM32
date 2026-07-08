/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "stm32c0xx_hal.h"

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
#define RCC_OSCX_IN_Pin GPIO_PIN_14
#define RCC_OSCX_IN_GPIO_Port GPIOC
#define RCC_OSCX_OUT_Pin GPIO_PIN_15
#define RCC_OSCX_OUT_GPIO_Port GPIOC
#define RCC_OSC_IN_Pin GPIO_PIN_0
#define RCC_OSC_IN_GPIO_Port GPIOF
#define RCC_OSC_OUT_Pin GPIO_PIN_1
#define RCC_OSC_OUT_GPIO_Port GPIOF
#define Analog_Pin GPIO_PIN_0
#define Analog_GPIO_Port GPIOA
#define SW_A1_Pin GPIO_PIN_1
#define SW_A1_GPIO_Port GPIOA
#define SW_A2_Pin GPIO_PIN_4
#define SW_A2_GPIO_Port GPIOA
#define LED_D1_Pin GPIO_PIN_5
#define LED_D1_GPIO_Port GPIOA
#define LED_D2_Pin GPIO_PIN_6
#define LED_D2_GPIO_Port GPIOA
#define LED_D3_Pin GPIO_PIN_7
#define LED_D3_GPIO_Port GPIOA
#define LED_D4_Pin GPIO_PIN_0
#define LED_D4_GPIO_Port GPIOB
#define SW_A3_Pin GPIO_PIN_1
#define SW_A3_GPIO_Port GPIOB
#define HW262_DISP_LATCH_Pin GPIO_PIN_10
#define HW262_DISP_LATCH_GPIO_Port GPIOB
#define HW262_DISP_DATA_Pin GPIO_PIN_9
#define HW262_DISP_DATA_GPIO_Port GPIOA
#define DEBUG_SWDIO_Pin GPIO_PIN_13
#define DEBUG_SWDIO_GPIO_Port GPIOA
#define DEBUG_SWCLK_Pin GPIO_PIN_14
#define DEBUG_SWCLK_GPIO_Port GPIOA
#define HW262_DISP_CLK_Pin GPIO_PIN_15
#define HW262_DISP_CLK_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
