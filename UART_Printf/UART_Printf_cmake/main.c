/**
  ******************************************************************************
  * file           : main.c
  * brief          : Main program body
  *                  Calls target system initialization then loop in main.
  ******************************************************************************
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32c562xx.h"
#include "stm32c5xx_hal_gpio.h"
#include "mx_usart2.h"
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/
static hal_uart_handle_t *uart;

/* Private functions prototype -----------------------------------------------*/
int __io_putchar(int ch);

/**
  * brief:  The application entry point.
  * retval: none but we specify int to comply with C99 standard
  */
int main(void)
{
  /* Initialize the system and peripherals configured in CubeMX2. */
  if (mx_system_init() != SYSTEM_OK)
  {
    return (-1);
  }
  else
  {
    /* Application initialization: runs once. */
    uart = mx_usart2_uart_gethandle();

  

    printf("\r\nHello World!\r\n");
    printf("PR TechTalk - UART printf demo\r\n");

    /* Application loop: repeats forever. */
    while (1)
    {
      HAL_GPIO_TogglePin(HAL_GPIOA, HAL_GPIO_PIN_5);
      printf("LED toggled\r\n");
      HAL_Delay(1000);
    }
  }
} /* end main */

/* Function definitions ------------------------------------------------------*/

/* The existing syscalls.c calls this function for each printf character. */
int __io_putchar(int ch)
{
  uint8_t character = (uint8_t)ch;

  if (HAL_UART_Transmit(uart, &character, 1, 100) != HAL_OK)
  {
    return EOF;
  }

  return ch;
}
