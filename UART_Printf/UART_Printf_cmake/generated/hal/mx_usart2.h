/**
  ******************************************************************************
  * @file           : mx_usart2.h
  * @brief          : Header for mx_usart2.c file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the mx_stm32c5xx_hal_drivers_license.md file
  * in the same directory as the generated code.
  * If no mx_stm32c5xx_hal_drivers_license.md file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef MX_USART2_H
#define MX_USART2_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/
#include "stm32_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** Primary aliases for USART2_RX pin */
#define PA3_PORT                              HAL_GPIOA
#define PA3_PIN                               HAL_GPIO_PIN_3

/** Primary aliases for USART2_TX pin */
#define PA2_PORT                              HAL_GPIOA
#define PA2_PIN                               HAL_GPIO_PIN_2

/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */

/******************************************************************************/
/* Exported functions for UART in HAL layer */
/******************************************************************************/
/**
  * @brief mx_usart2_uart init function
  * This function configures the hardware resources used in this example
  * @retval pointer to handle or NULL in case of failure
  */
hal_uart_handle_t *mx_usart2_uart_init(void);

/**
  * @brief  De-initialize mx_usart2_uart instance and return it.
  * @retval None
  */
void mx_usart2_uart_deinit(void);

/**
  * @brief  Get the mx_usart2_uart object.
  * @retval Pointer on the mx_usart2_uartHandle
  */
hal_uart_handle_t *mx_usart2_uart_gethandle(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX_USART2_H */
