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
#include "stm32u0xx_hal.h"

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
#define LED_GREEN_Pin GPIO_PIN_13
#define LED_GREEN_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOF
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOF
#define Joystick_Pin GPIO_PIN_2
#define Joystick_GPIO_Port GPIOC
#define VCP_TX_Pin GPIO_PIN_2
#define VCP_TX_GPIO_Port GPIOA
#define VCP_RX_Pin GPIO_PIN_3
#define VCP_RX_GPIO_Port GPIOA
#define LED_BLUE_Pin GPIO_PIN_5
#define LED_BLUE_GPIO_Port GPIOA
#define SEG0_Pin GPIO_PIN_4
#define SEG0_GPIO_Port GPIOC
#define SEG1_Pin GPIO_PIN_5
#define SEG1_GPIO_Port GPIOC
#define SEG2_Pin GPIO_PIN_1
#define SEG2_GPIO_Port GPIOB
#define LED_RED_Pin GPIO_PIN_2
#define LED_RED_GPIO_Port GPIOB
#define SEG3_Pin GPIO_PIN_7
#define SEG3_GPIO_Port GPIOE
#define SEG4_Pin GPIO_PIN_8
#define SEG4_GPIO_Port GPIOE
#define SEG5_Pin GPIO_PIN_9
#define SEG5_GPIO_Port GPIOE
#define SEG6_Pin GPIO_PIN_11
#define SEG6_GPIO_Port GPIOB
#define SHIELD_Pin GPIO_PIN_12
#define SHIELD_GPIO_Port GPIOB
#define SHIELD_CS_Pin GPIO_PIN_13
#define SHIELD_CS_GPIO_Port GPIOB
#define SEG7_Pin GPIO_PIN_14
#define SEG7_GPIO_Port GPIOB
#define SEG8_Pin GPIO_PIN_15
#define SEG8_GPIO_Port GPIOB
#define SEG9_Pin GPIO_PIN_8
#define SEG9_GPIO_Port GPIOD
#define SEG10_Pin GPIO_PIN_9
#define SEG10_GPIO_Port GPIOD
#define TEKY_CS_Pin GPIO_PIN_10
#define TEKY_CS_GPIO_Port GPIOD
#define TEKY_Pin GPIO_PIN_11
#define TEKY_GPIO_Port GPIOD
#define SEG11_Pin GPIO_PIN_12
#define SEG11_GPIO_Port GPIOD
#define SEG12_Pin GPIO_PIN_13
#define SEG12_GPIO_Port GPIOD
#define SEG13_Pin GPIO_PIN_6
#define SEG13_GPIO_Port GPIOC
#define SEG14_Pin GPIO_PIN_8
#define SEG14_GPIO_Port GPIOC
#define SEG15_Pin GPIO_PIN_9
#define SEG15_GPIO_Port GPIOC
#define COM0_Pin GPIO_PIN_8
#define COM0_GPIO_Port GPIOA
#define COM1_Pin GPIO_PIN_9
#define COM1_GPIO_Port GPIOA
#define COM2_Pin GPIO_PIN_10
#define COM2_GPIO_Port GPIOA
#define USB_DM_Pin GPIO_PIN_11
#define USB_DM_GPIO_Port GPIOA
#define USB_DP_Pin GPIO_PIN_12
#define USB_DP_GPIO_Port GPIOA
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define SEG16_Pin GPIO_PIN_10
#define SEG16_GPIO_Port GPIOC
#define SEG23_Pin GPIO_PIN_11
#define SEG23_GPIO_Port GPIOC
#define SEG17_Pin GPIO_PIN_0
#define SEG17_GPIO_Port GPIOD
#define SEG18_Pin GPIO_PIN_1
#define SEG18_GPIO_Port GPIOD
#define SEG19_Pin GPIO_PIN_3
#define SEG19_GPIO_Port GPIOD
#define SEG20_Pin GPIO_PIN_4
#define SEG20_GPIO_Port GPIOD
#define SEG21_Pin GPIO_PIN_5
#define SEG21_GPIO_Port GPIOD
#define SEG22_Pin GPIO_PIN_6
#define SEG22_GPIO_Port GPIOD
#define SDA_Pin GPIO_PIN_7
#define SDA_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define COM3_Pin GPIO_PIN_9
#define COM3_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
