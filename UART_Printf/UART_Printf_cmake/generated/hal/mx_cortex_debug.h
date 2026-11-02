/**
  ******************************************************************************
  * @file           : mx_cortex_debug.h
  * @brief          : Header for mx_cortex_debug.c file.
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
#ifndef MX_CORTEX_DEBUG_H
#define MX_CORTEX_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/* Includes ------------------------------------------------------------------*/

#include "stm32_hal.h"
#include "mx_def.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/

/** Primary aliases for DEBUG_SWCLK pin */
#define DBG_SWCLK_PORT                        HAL_GPIOA
#define DBG_SWCLK_PIN                         HAL_GPIO_PIN_14

/** Primary aliases for DEBUG_SWDIO pin */
#define DBG_SWDIO_PORT                        HAL_GPIOA
#define DBG_SWDIO_PIN                         HAL_GPIO_PIN_13
/* Exported macros -----------------------------------------------------------*/
/* Exported variables --------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
/******************************************************************************/
/* Exported functions for CORTEX_DEBUG in HAL layer */
/******************************************************************************/
/**
  * @brief mx_cortex_debug init function
  * This function configures the hardware resources used in this example
  * @retval pointer to handle or NULL in case of failure
  */
system_status_t mx_cortex_debug_init(void);

/**
  * @brief  De-initialize cortex_debug instance and return it.
  */
void mx_cortex_debug_deinit(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* MX_CORTEX_DEBUG_H */
