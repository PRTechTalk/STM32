/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <string.h>
#include <stdio.h>
#include "ssd1306.h"
#include "ssd1306_fonts.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define ESC "\033"
#define CLEAR_SCREEN ESC "[2J"
#define CURSOR_HOME ESC "[H"


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

RTC_HandleTypeDef hrtc;

TIM_HandleTypeDef htim6;

UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_I2C1_Init(void);
static void MX_RTC_Init(void);
static void MX_TIM6_Init(void);
/* USER CODE BEGIN PFP */
void Set_RTC_Time(void);
void Set_RTC_Date(void);
void Set_RTC_Alarm(void);
void Show_Menu(void);
void Print_Time(void);
void Print_Date(void);
void Print_Alarm(void);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc);
void UART_Receive(uint8_t *data, uint16_t size);
void Clear_Screen(void);
void Print_Date_Oled(void);
void Print_Time_Oled(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void Clear_Screen(void)
{
    HAL_UART_Transmit(&huart2, (uint8_t*)CLEAR_SCREEN, strlen(CLEAR_SCREEN), HAL_MAX_DELAY);
    HAL_UART_Transmit(&huart2, (uint8_t*)CURSOR_HOME, strlen(CURSOR_HOME), HAL_MAX_DELAY);
}

void UART_Receive(uint8_t *buffer, uint16_t size)
{
    // Ensure the buffer is null-terminated
    HAL_UART_Receive(&huart2, buffer, size, HAL_MAX_DELAY);
    buffer[size] = '\0';
}

void Set_RTC_Time(void)
{
    RTC_TimeTypeDef sTime = {0};
    uint8_t buffer[3];



    HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Hours (00-23): ", 21, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sTime.Hours = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Minutes (00-59): ", 24, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sTime.Minutes = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Seconds (00-59): \n\r", 24, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sTime.Seconds = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sTime.StoreOperation = RTC_STOREOPERATION_RESET;

    if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rTime is set!\n\r ", 16, HAL_MAX_DELAY);
}

void Set_RTC_Date(void)
{
    RTC_DateTypeDef sDate = {0};
    uint8_t buffer[3];



    HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Year (00-99): ", 20, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sDate.Year = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Month (01-12): ", 22, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sDate.Month = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Date (01-31): ", 21, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sDate.Date = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Weekday (1=Mon, 7=Sun): ", 33, HAL_MAX_DELAY);
    UART_Receive(buffer, 1);
    sDate.WeekDay = buffer[0] - '0';

    if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rDate is set!\n\r ", 16, HAL_MAX_DELAY);

}

void Set_RTC_Alarm(void)
{
    RTC_AlarmTypeDef sAlarm = {0};
    uint8_t buffer[3];


    HAL_UART_Transmit(&huart2, (uint8_t*)"Enter Alarm Hours (00-23): ", 27, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sAlarm.AlarmTime.Hours = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Alarm Minutes (00-59): ", 30, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sAlarm.AlarmTime.Minutes = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rEnter Alarm Seconds (00-59): ", 30, HAL_MAX_DELAY);
    UART_Receive(buffer, 2);
    sAlarm.AlarmTime.Seconds = (buffer[0] - '0') * 10 + (buffer[1] - '0');

    sAlarm.AlarmTime.SubSeconds = 0;
    sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
    sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
    sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
    sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
    sAlarm.AlarmDateWeekDay = 0x01;
    sAlarm.Alarm = RTC_ALARM_A;

    if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK)
    {
        Error_Handler();
    }
    HAL_UART_Transmit(&huart2, (uint8_t*)"\n\rAlarm is set!\n\r", 17, HAL_MAX_DELAY);
}

void Print_Time(void)
{
    RTC_TimeTypeDef sTime;
    char buffer[50];

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    sprintf(buffer, "Current Time: %02d:%02d:%02d\n\r", sTime.Hours, sTime.Minutes, sTime.Seconds);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void Print_Date(void)
{
    RTC_DateTypeDef sDate;
    char buffer[50];

    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    sprintf(buffer, "Current Date: %02d-%02d-20%02d\n\r", sDate.Date, sDate.Month, sDate.Year);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void Print_Date_Oled(void)
{
    RTC_DateTypeDef sDate;
    char buffer_oled[15] ={0};

    HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BIN);
    sprintf(buffer_oled, "%02d-%02d-20%02d", sDate.Date, sDate.Month, sDate.Year);
    ssd1306_SetCursor(15, 45);
    ssd1306_WriteString(buffer_oled, Font_16x15, White);
    ssd1306_UpdateScreen();
}

void Print_Time_Oled(void)
{
    RTC_TimeTypeDef sTime;
    char buffer_oled[15] ={0};

    HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BIN);
    sprintf(buffer_oled, "%02d:%02d:%02d", sTime.Hours, sTime.Minutes, sTime.Seconds);
    ssd1306_SetCursor(20, 28);
    ssd1306_WriteString(buffer_oled, Font_11x18, White);
    ssd1306_UpdateScreen();
}

void Print_Alarm(void)
{
    RTC_AlarmTypeDef sAlarm;
    char buffer[50];

    HAL_RTC_GetAlarm(&hrtc, &sAlarm, RTC_ALARM_A, RTC_FORMAT_BIN);
    sprintf(buffer, "Alarm Time: %02d:%02d:%02d\n\r", sAlarm.AlarmTime.Hours, sAlarm.AlarmTime.Minutes, sAlarm.AlarmTime.Seconds);
    HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void Show_Menu(void)
{
    Clear_Screen();
    const char *menu = "\r\nMenu:\r\n"
    				   "0. Show Menu\r\n"
                       "1. Show Time\r\n"
                       "2. Show Date\r\n"
                       "3. Show Alarm\r\n"
                       "4. Set Time\r\n"
                       "5. Set Date\r\n"
                       "6. Set Alarm\r\n"
    				   "7. Reset Alarm Led\r\n\n";

    HAL_UART_Transmit(&huart2, (uint8_t*)menu, strlen(menu), HAL_MAX_DELAY);
}


/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */
	char OledText[] = "PR TechTalk RTC";

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
  MX_I2C1_Init();
  MX_RTC_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
  ssd1306_Init();
  ssd1306_SetCursor(0, 0);
  ssd1306_Fill(Black);
  ssd1306_WriteString(OledText, Font_11x18, White);
  ssd1306_Line(0, 20, 128, 20, White);
  ssd1306_UpdateScreen();
  Show_Menu();

  // Start the TIM6 interrupt
  HAL_TIM_Base_Start_IT(&htim6);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	  uint8_t userChoice = 0;

	  	  // Wait for user input
	  	  UART_Receive(&userChoice, 1);

	  	  // Handle user input
	  	  	          switch (userChoice)
	  	  	          {

	  	  	          	  case '0':
	  	  	        	  	  Show_Menu();
	  	  	        	  	  break;
	  	  	          	  case '1':
	  	  	                  Print_Time();
	  	  	                  break;
	  	  	              case '2':
	  	  	                  Print_Date();
	  	  	                  break;
	  	  	              case '3':
	  	  	                  Print_Alarm();
	  	  	                  break;
	  	  	              case '4':
	  	  	                  Set_RTC_Time();
	  	  	                  break;
	  	  	              case '5':
	  	  	                  Set_RTC_Date();
	  	  	                  break;
	  	  	              case '6':
	  	  	                  Set_RTC_Alarm();
	  	  	                  break;
	  	  	              case '7':
	  	  	            	  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);
	  	  	              default:
	  	  	                  HAL_UART_Transmit(&huart2, (uint8_t*)"Invalid choice. Please try again.\n\r", 36, HAL_MAX_DELAY);
	  	  	                  break;
	  	  	          }

	  	  	          // After user input is processed, show menu again
	  	  	          //  Show_Menu();






    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();
  __HAL_RCC_LSEDRIVE_CONFIG(RCC_LSEDRIVE_LOW);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10B17DB5;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief RTC Initialization Function
  * @param None
  * @retval None
  */
static void MX_RTC_Init(void)
{

  /* USER CODE BEGIN RTC_Init 0 */

  /* USER CODE END RTC_Init 0 */

  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};
  RTC_AlarmTypeDef sAlarm = {0};

  /* USER CODE BEGIN RTC_Init 1 */

  /* USER CODE END RTC_Init 1 */

  /** Initialize RTC Only
  */
  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutRemap = RTC_OUTPUT_REMAP_NONE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  hrtc.Init.OutPutPullUp = RTC_OUTPUT_PULLUP_NONE;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  /* USER CODE BEGIN Check_RTC_BKUP */

  /* USER CODE END Check_RTC_BKUP */

  /** Initialize RTC and set the Time and Date
  */
  sTime.Hours = 0x0;
  sTime.Minutes = 0x0;
  sTime.Seconds = 0x0;
  sTime.SubSeconds = 0x0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_JANUARY;
  sDate.Date = 0x1;
  sDate.Year = 0x0;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }

  /** Enable the Alarm A
  */
  sAlarm.AlarmTime.Hours = 0x0;
  sAlarm.AlarmTime.Minutes = 0x0;
  sAlarm.AlarmTime.Seconds = 0x0;
  sAlarm.AlarmTime.SubSeconds = 0x0;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  sAlarm.AlarmMask = RTC_ALARMMASK_NONE;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay = 0x1;
  sAlarm.Alarm = RTC_ALARM_A;
  if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RTC_Init 2 */

  /* USER CODE END RTC_Init 2 */

}

/**
  * @brief TIM6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM6_Init(void)
{

  /* USER CODE BEGIN TIM6_Init 0 */

  /* USER CODE END TIM6_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM6_Init 1 */

  /* USER CODE END TIM6_Init 1 */
  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 64000-1;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 1000-1;
  htim6.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim6) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM6_Init 2 */

  /* USER CODE END TIM6_Init 2 */

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
  if (HAL_UARTEx_SetTxFifoThreshold(&huart2, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart2, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart2) != HAL_OK)
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
  HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED_GREEN_Pin */
  GPIO_InitStruct.Pin = LED_GREEN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(LED_GREEN_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);

}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM6)
    {
        // Your 1-second interrupt handling code here
    	Print_Date_Oled();
    	Print_Time_Oled();
    }
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

#ifdef  USE_FULL_ASSERT
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
