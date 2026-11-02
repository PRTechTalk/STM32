
/**
  ******************************************************************************
  * @file           : mx_cortex_debug.c
  * @brief          : CORTEX_DEBUG Peripheral initialization
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

/* Includes ------------------------------------------------------------------*/
#include "mx_cortex_debug.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
/* Exported variables by reference--------------------------------------------*/

/******************************************************************************/
/* Exported functions for CORTEX_DEBUG in HAL layer */
/******************************************************************************/
system_status_t mx_cortex_debug_init(void)
{
  /* ### CM33 GPIO Configuration ########################### */
  /* GPIO Clocks activation */
  HAL_RCC_GPIOA_EnableClock();

  hal_gpio_config_t  gpio_config;

  /**
    [GPIO Pin] ------> [Signal Name] ------> [Labels]

       PA14    ------>   DEBUG_SWCLK   ------>  DBG_SWCLK
    **/
  gpio_config.mode        = HAL_GPIO_MODE_ALTERNATE;
  gpio_config.output_type = HAL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.pull        = HAL_GPIO_PULL_DOWN;
  gpio_config.speed       = HAL_GPIO_SPEED_FREQ_LOW;
  gpio_config.alternate   = HAL_GPIO_AF_0;
  HAL_GPIO_Init(DBG_SWCLK_PORT, DBG_SWCLK_PIN, &gpio_config);

  /**
    [GPIO Pin] ------> [Signal Name] ------> [Labels]

       PA13    ------>   DEBUG_SWDIO   ------>  DBG_SWDIO
    **/
  gpio_config.mode        = HAL_GPIO_MODE_ALTERNATE;
  gpio_config.output_type = HAL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.pull        = HAL_GPIO_PULL_UP;
  gpio_config.speed       = HAL_GPIO_SPEED_FREQ_VERY_HIGH;
  gpio_config.alternate   = HAL_GPIO_AF_0;
  HAL_GPIO_Init(DBG_SWDIO_PORT, DBG_SWDIO_PIN, &gpio_config);

  return SYSTEM_OK;
}

void mx_cortex_debug_deinit(void)
{
  /* De-initialize all GPIOA pins associated with CM33 */
  HAL_GPIO_DeInit(HAL_GPIOA, DBG_SWDIO_PIN | DBG_SWCLK_PIN);
}
