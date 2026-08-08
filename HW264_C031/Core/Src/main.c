/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "hw262_display.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define DEMO_UPDATE_PERIOD_MS  100U
#define DEBUG_UPDATE_PERIOD_MS 500U
#define LED_COUNT              4U
#define ADC_FULL_SCALE         4095U
#define BUZZER_MIN_HZ          500U
#define BUZZER_MAX_HZ          2500U
#define BUZZER_TIMER_HZ        1000000U

/* The switches on HW-264 pull their inputs low while pressed. */
#define SWITCH_PRESSED         GPIO_PIN_RESET

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */

static uint16_t Demo_ReadAnalog(void);
static void Demo_SetLedBar(uint16_t adc_value);
static void Demo_SetAllLeds(GPIO_PinState state);
static void Demo_RunLedChase(uint32_t now);
static void Demo_SetBuzzer(uint32_t frequency_hz);
static void Debug_SendStatus(uint16_t analog_value, GPIO_PinState sw1,
                             GPIO_PinState sw2, GPIO_PinState sw3);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  Demo_SetAllLeds(GPIO_PIN_RESET);
  Demo_SetBuzzer(0U);
  HW262_Display_Init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    HW262_Display_Process();
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
    static uint32_t last_update = 0U;
    uint32_t now = HAL_GetTick();

    if ((now - last_update) >= DEMO_UPDATE_PERIOD_MS)
    {
      static uint32_t last_debug_update = 0U;
      uint16_t analog_value;
      GPIO_PinState sw1;
      GPIO_PinState sw2;
      GPIO_PinState sw3;

      last_update = now;
      analog_value = Demo_ReadAnalog();
      HW262_Display_SetNumber(analog_value, false);
      sw1 = HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);
      sw2 = HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
      sw3 = HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin);

      if (sw3 == SWITCH_PRESSED)
      {
        /* SW3: pot controls the buzzer from 500 to 2500 Hz. */
        uint32_t frequency = BUZZER_MIN_HZ
                           + (((uint32_t)analog_value
                           * (BUZZER_MAX_HZ - BUZZER_MIN_HZ))
                           / ADC_FULL_SCALE);
        Demo_SetLedBar(analog_value);
        Demo_SetBuzzer(frequency);
      }
      else if (sw2 == SWITCH_PRESSED)
      {
        /* SW2: running-light test for all four LEDs. */
        Demo_SetBuzzer(0U);
        Demo_RunLedChase(now);
      }
      else if (sw1 == SWITCH_PRESSED)
      {
        /* SW1: turn on all LEDs. */
        Demo_SetBuzzer(0U);
        Demo_SetAllLeds(GPIO_PIN_SET);
      }
      else
      {
        /* Default: show the potentiometer value as an LED bar. */
        Demo_SetBuzzer(0U);
        Demo_SetLedBar(analog_value);
      }

      if ((now - last_debug_update) >= DEBUG_UPDATE_PERIOD_MS)
      {
        last_debug_update = now;
        Debug_SendStatus(analog_value, sw1, sw2, sw3);
      }
    }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  __HAL_FLASH_SET_LATENCY(FLASH_LATENCY_1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSE;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.ScanConvMode = ADC_SCAN_SEQ_FIXED;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.LowPowerAutoPowerOff = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.SamplingTimeCommon1 = ADC_SAMPLETIME_1CYCLE_5;
  hadc1.Init.OversamplingMode = DISABLE;
  hadc1.Init.TriggerFrequencyMode = ADC_TRIGGER_FREQ_HIGH;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 48-1;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 500-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 250;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_LOW;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */
  HAL_TIM_MspPostInit(&htim3);

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOF_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, LED1_Pin|LED2_Pin|LED3_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED4_GPIO_Port, LED4_Pin, GPIO_PIN_SET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(HW262_DISP_LATCH_GPIO_Port, HW262_DISP_LATCH_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, HW262_DISP_DATA_Pin|HW262_DISP_CLK_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SW1_Pin SW2_Pin */
  GPIO_InitStruct.Pin = SW1_Pin|SW2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin HW262_DISP_DATA_Pin
                           HW262_DISP_CLK_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|HW262_DISP_DATA_Pin
                          |HW262_DISP_CLK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : LED4_Pin HW262_DISP_LATCH_Pin */
  GPIO_InitStruct.Pin = LED4_Pin|HW262_DISP_LATCH_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : SW3_Pin */
  GPIO_InitStruct.Pin = SW3_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SW3_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

static uint16_t Demo_ReadAnalog(void)
{
  uint16_t value = 0U;

  if (HAL_ADC_Start(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ADC_PollForConversion(&hadc1, 5U) == HAL_OK)
  {
    value = (uint16_t)HAL_ADC_GetValue(&hadc1);
  }
  else
  {
    Error_Handler();
  }
  if (HAL_ADC_Stop(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  return value;
}

static void Demo_WriteLed(uint32_t index, GPIO_PinState state)
{
  static GPIO_TypeDef *const ports[LED_COUNT] =
  {
    LED1_GPIO_Port, LED2_GPIO_Port, LED3_GPIO_Port, LED4_GPIO_Port
  };
  static const uint16_t pins[LED_COUNT] =
  {
    LED1_Pin, LED2_Pin, LED3_Pin, LED4_Pin
  };

  if (index < LED_COUNT)
  {
    /* LEDs are active low, so invert the logical on/off state. */
    HAL_GPIO_WritePin(ports[index], pins[index],
                      (state == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET);
  }
}

static void Demo_SetAllLeds(GPIO_PinState state)
{
  uint32_t index;

  for (index = 0U; index < LED_COUNT; index++)
  {
    Demo_WriteLed(index, state);
  }
}

static void Demo_SetLedBar(uint16_t adc_value)
{
  uint32_t index;
  uint32_t lit_leds = (((uint32_t)adc_value * LED_COUNT)
                      / (ADC_FULL_SCALE + 1U));

  /* Make full scale light all four LEDs. */
  if (adc_value >= (ADC_FULL_SCALE - 3U))
  {
    lit_leds = LED_COUNT;
  }

  for (index = 0U; index < LED_COUNT; index++)
  {
    Demo_WriteLed(index, (index < lit_leds) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

static void Demo_RunLedChase(uint32_t now)
{
  uint32_t index;
  uint32_t active_led = (now / 150U) % LED_COUNT;

  for (index = 0U; index < LED_COUNT; index++)
  {
    Demo_WriteLed(index,
                  (index == active_led) ? GPIO_PIN_SET : GPIO_PIN_RESET);
  }
}

static void Demo_SetBuzzer(uint32_t frequency_hz)
{
  static uint32_t current_frequency = UINT32_MAX;

  if (frequency_hz == current_frequency)
  {
    return;
  }

  if (frequency_hz == 0U)
  {
    (void)HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_2);
  }
  else
  {
    uint32_t period = BUZZER_TIMER_HZ / frequency_hz;

    __HAL_TIM_SET_AUTORELOAD(&htim3, period - 1U);
    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_2, period / 2U);
    if (current_frequency == 0U || current_frequency == UINT32_MAX)
    {
      __HAL_TIM_SET_COUNTER(&htim3, 0U);
      if (HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_2) != HAL_OK)
      {
        Error_Handler();
      }
    }
  }

  current_frequency = frequency_hz;
}

static char *Debug_AppendUnsigned(char *destination, uint32_t value)
{
  char reversed[10];
  uint32_t length = 0U;

  do
  {
    reversed[length++] = (char)('0' + (value % 10U));
    value /= 10U;
  } while (value != 0U);

  while (length > 0U)
  {
    *destination++ = reversed[--length];
  }

  return destination;
}

static char *Debug_AppendText(char *destination, const char *text)
{
  while (*text != '\0')
  {
    *destination++ = *text++;
  }

  return destination;
}

static void Debug_SendStatus(uint16_t analog_value, GPIO_PinState sw1,
                             GPIO_PinState sw2, GPIO_PinState sw3)
{
  char message[72];
  char *write_position = message;
  const char *mode;

  if (sw3 == SWITCH_PRESSED)
  {
    mode = "BUZZER";
  }
  else if (sw2 == SWITCH_PRESSED)
  {
    mode = "CHASE";
  }
  else if (sw1 == SWITCH_PRESSED)
  {
    mode = "ALL_LED";
  }
  else
  {
    mode = "POT_BAR";
  }

  /* ANSI: clear the terminal and move the cursor to the top-left corner. */
  write_position = Debug_AppendText(write_position, "\x1B[2J\x1B[H");
  write_position = Debug_AppendText(write_position, "ADC=");
  write_position = Debug_AppendUnsigned(write_position, analog_value);
  write_position = Debug_AppendText(write_position, " SW1=");
  *write_position++ = (sw1 == SWITCH_PRESSED) ? '1' : '0';
  write_position = Debug_AppendText(write_position, " SW2=");
  *write_position++ = (sw2 == SWITCH_PRESSED) ? '1' : '0';
  write_position = Debug_AppendText(write_position, " SW3=");
  *write_position++ = (sw3 == SWITCH_PRESSED) ? '1' : '0';
  write_position = Debug_AppendText(write_position, " MODE=");
  write_position = Debug_AppendText(write_position, mode);
  *write_position++ = '\r';
  *write_position++ = '\n';

  (void)HAL_UART_Transmit(&huart2, (uint8_t *)message,
                          (uint16_t)(write_position - message), 20U);
}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
