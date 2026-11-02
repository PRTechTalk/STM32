/**
  ******************************************************************************
  * @file           : mx_usart2.c
  * @brief          : USART2 Peripheral initialization
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
#include "mx_usart2.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions prototype------------------------------------------------*/
/* Exported variables by reference--------------------------------------------*/
/* Handle for UART */
static hal_uart_handle_t hUSART2;

/* Exported function definition ----------------------------------------------*/
/******************************************************************************/
/* Exported functions for UART in HAL layer */
/******************************************************************************/

hal_uart_handle_t *mx_usart2_uart_init(void)
{
  hal_uart_config_t uart_config;

  /* Basic configuration */
  if (HAL_UART_Init(&hUSART2, HAL_UART2) != HAL_OK)
  {
    return NULL;
  }

  HAL_RCC_USART2_EnableClock();

  if (HAL_RCC_USART2_SetKernelClkSource(HAL_RCC_USART2_CLK_SRC_PCLK1) != HAL_OK)
  {
    return NULL;
  }

  uart_config.baud_rate = 115200;
  uart_config.clock_prescaler = HAL_UART_PRESCALER_DIV1;
  uart_config.word_length = HAL_UART_WORD_LENGTH_8_BIT;
  uart_config.stop_bits = HAL_UART_STOP_BIT_1;
  uart_config.parity = HAL_UART_PARITY_NONE;
  uart_config.direction = HAL_UART_DIRECTION_TX_RX;
  uart_config.hw_flow_ctl = HAL_UART_HW_CONTROL_NONE;
  uart_config.oversampling = HAL_UART_OVERSAMPLING_16;
  uart_config.one_bit_sampling = HAL_UART_ONE_BIT_SAMPLE_DISABLE;

  if (HAL_UART_SetConfig(&hUSART2, &uart_config) != HAL_OK)
  {
    return NULL;
  }

  /* ### USART2 GPIO Configuration ########################### */
  /* GPIO Clocks activation */
  HAL_RCC_GPIOA_EnableClock();

  hal_gpio_config_t  gpio_config;

  /**
    [GPIO Pin] ------> [Signal Name] ------> [Labels]

       PA3     ------>   USART2_RX   ------>  PA3
       PA2     ------>   USART2_TX   ------>  PA2
    **/
  gpio_config.mode        = HAL_GPIO_MODE_ALTERNATE;
  gpio_config.output_type = HAL_GPIO_OUTPUT_PUSHPULL;
  gpio_config.pull        = HAL_GPIO_PULL_NO;
  gpio_config.speed       = HAL_GPIO_SPEED_FREQ_LOW;
  gpio_config.alternate   = HAL_GPIO_AF_7;
  HAL_GPIO_Init(HAL_GPIOA, PA3_PIN | PA2_PIN, &gpio_config);

  return &hUSART2;
}

void mx_usart2_uart_deinit(void)
{
(void)HAL_UART_DeInit(&hUSART2);

  HAL_RCC_USART2_Reset();

  HAL_RCC_USART2_DisableClock();

  /* De-initialize all GPIOA pins associated with USART2 */
  HAL_GPIO_DeInit(HAL_GPIOA, PA2_PIN | PA3_PIN);
}
hal_uart_handle_t *mx_usart2_uart_gethandle(void)
{
  return &hUSART2;
}
