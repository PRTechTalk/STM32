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
#include "mongoose_glue.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define WIFISPI SPI1
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

RNG_HandleTypeDef hrng;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void SystemPower_Config(void);
static void MX_GPIO_Init(void);
static void MX_ICACHE_Init(void);
static void MX_RNG_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#define BIT(x) (1UL << (x))
#define CLRSET(reg, clear, set) ((reg) = ((reg) & ~(clear)) | (set))

static inline void spin(volatile uint32_t n) {
  while (n--)
    (void)0;
}

static inline void hal_spi_init(SPI_TypeDef *spi, uint16_t div) {
  (void)spi;
  uint32_t br = 0; // Baud rate prescaler: div = 2,4,8...256 -> MBR = 0..7
  while (div > 2)
    div >>= 1, br++;
  if (br > 7)
    br = 7;


  CLRSET(RCC->CCIPR1, 3 << 20, 1); // SYSCLK source
  RCC->APB2ENR |= BIT(12); // SPI1EN
  spin(100);
  spi->CR1 = 0;                  // disable SPI
  spi->CR1 = SPI_CR1_SSI_Msk; // SSI
  spi->CFG1 = (7 << SPI_CFG1_DSIZE_Pos) | (br << SPI_CFG1_MBR_Pos); // DSIZE = 8-bit, MBR
  spi->CFG2 = SPI_CFG2_SSM_Msk | SPI_CFG2_MASTER_Msk;
  spi->CR1 |= SPI_CR1_SPE_Msk; // Enable SPI
  spi->CR1 |= SPI_CR1_CSTART_Msk; // Start transfer
}

static inline uint8_t hal_spi_xfer_byte(SPI_TypeDef *spi, uint8_t byte) {
  (void)spi;
  while ((spi->SR & SPI_SR_TXP_Msk) == 0)
    spin(1); // TXP
  *(volatile uint8_t *) &spi->TXDR = byte;
  while (((spi->SR & SPI_SR_RXP_Msk) == 0) && ((spi->SR & SPI_SR_RXWNE_Msk) == 0) && ((spi->SR & SPI_SR_RXPLVL_Msk) == 0))
    spin(1);        // RXP
  uint8_t rx = *(volatile uint8_t *) &spi->RXDR;
  return rx;
}

static inline void hal_spi_tx_rx(SPI_TypeDef *spi, uint8_t *tx, uint8_t *rx, size_t len) {
  for (size_t i = 0; i < len; i++) {
    uint8_t rx_ = hal_spi_xfer_byte(spi, tx ? tx[i] : 0);
    if (rx) rx[i] = rx_;
  }
}

static inline bool hal_spi_ready(SPI_TypeDef *spi) {
  (void) spi;
  return HAL_GPIO_ReadPin(SPI_RDY_GPIO_Port, SPI_RDY_Pin);
}

static inline void hal_spi_select(SPI_TypeDef *spi, bool s) {
  (void)spi;
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, s);
}

static inline void hal_spi_done(SPI_TypeDef *spi) {
  while((spi->SR & SPI_SR_TXC_Msk) == 0) spin(1);
  spi->IFCR = 0xffff; // clear all flags
}

static bool hwspecific_is_ready(void) {
  return hal_spi_ready(WIFISPI);
}

static void hwspecific_spi_begin(void *arg) {
  hal_spi_select(WIFISPI, true);
  (void) arg;
}

static void hwspecific_spi_txn(void *arg, uint8_t *txdata, uint8_t *rxdata, size_t len) {
  while (len--) {
    uint8_t rx = hal_spi_xfer_byte(WIFISPI, txdata != NULL ? *txdata++ : 0);
    if (rxdata != NULL) *rxdata++ = rx;
  }
  hal_spi_done(WIFISPI);
  (void) arg;
}

static void hwspecific_spi_end(void *arg) {
  hal_spi_select(WIFISPI, false);
  (void) arg;
}

void hwspecific_spi_init(void) {
  HAL_GPIO_WritePin(SPI_CHIP_EN_GPIO_Port, SPI_CHIP_EN_Pin, 0);
  hal_spi_init(WIFISPI, 128); // runs at 1.25MHz (SYS_CLK/128)
  mg_delayms(10);
  HAL_GPIO_WritePin(SPI_CHIP_EN_GPIO_Port, SPI_CHIP_EN_Pin, 1);
}

static const struct mg_tcpip_spi spi = {NULL, hwspecific_spi_begin, hwspecific_spi_end, hwspecific_spi_txn};

void wifi_setconfig(void *data) {
  struct mg_tcpip_driver_st67w6_data *d = (struct mg_tcpip_driver_st67w6_data *) data;
  d->wifi.ssid = WIZARD_WIFI_NAME;
  d->wifi.pass = WIZARD_WIFI_PASS;
  d->wifi.apssid = WIZARD_WIFI_AP_NAME;
  d->wifi.appass = WIZARD_WIFI_AP_PASS;
  d->wifi.apip = MG_IPV4(192, 168, 169, 1);
  d->wifi.apmask = MG_IPV4(255, 255, 255, 0);
  d->wifi.security = 0;
  d->wifi.apsecurity = 0;
  d->wifi.apchannel = 10;
  d->wifi.apmode = WIZARD_ENABLE_WIFI_AP;
  d->spi = (struct mg_tcpip_spi *) &spi;
  d->is_ready = hwspecific_is_ready;
}

bool mg_random(void *buf, size_t len) {  // Use on-board RNG
  for (size_t n = 0; n < len; n += sizeof(uint32_t)) {
    uint32_t r;
    HAL_RNG_GenerateRandomNumber(&hrng, &r);
    memcpy((char *) buf + n, &r, n + sizeof(r) > len ? len - n : sizeof(r));
  }
  return true; // TODO(): ensure successful RNG init, then return on false above
}

uint64_t mg_millis(void) {
  return HAL_GetTick();
}

int _write(int fd, unsigned char *buf, int len) {
  if (fd == 1 || fd == 2) {                     // stdout or stderr ?
    HAL_UART_Transmit(&huart1, buf, len, 999);  // Print to the UART
  }
  return len;
}


void my_get_leds(struct leds *data) {
  data->led1 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_0);
  //data->led2 = HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_7);
}

void my_set_leds(struct leds *data) {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, data->led1);
  //HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7, data->led2);
}
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

  /* Configure the System Power */
  SystemPower_Config();

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ICACHE_Init();
  MX_RNG_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
hwspecific_spi_init();
mongoose_init();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    mongoose_poll();

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
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI48|RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.HSI48State = RCC_HSI48_ON;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = RCC_MSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_0;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLMBOOST = RCC_PLLMBOOST_DIV4;
  RCC_OscInitStruct.PLL.PLLM = 3;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 1;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLLVCIRANGE_1;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_PCLK3;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief Power Configuration
  * @retval None
  */
static void SystemPower_Config(void)
{

  /*
   * Switch to SMPS regulator instead of LDO
   */
  if (HAL_PWREx_ConfigSupply(PWR_SMPS_SUPPLY) != HAL_OK)
  {
    Error_Handler();
  }
/* USER CODE BEGIN PWR */
/* USER CODE END PWR */
}

/**
  * @brief ICACHE Initialization Function
  * @param None
  * @retval None
  */
static void MX_ICACHE_Init(void)
{

  /* USER CODE BEGIN ICACHE_Init 0 */

  /* USER CODE END ICACHE_Init 0 */

  /* USER CODE BEGIN ICACHE_Init 1 */

  /* USER CODE END ICACHE_Init 1 */

  /** Enable instruction cache in 1-way (direct mapped cache)
  */
  if (HAL_ICACHE_ConfigAssociativityMode(ICACHE_1WAY) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_ICACHE_Enable() != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ICACHE_Init 2 */

  /* USER CODE END ICACHE_Init 2 */

}

/**
  * @brief RNG Initialization Function
  * @param None
  * @retval None
  */
static void MX_RNG_Init(void)
{

  /* USER CODE BEGIN RNG_Init 0 */

  /* USER CODE END RNG_Init 0 */

  /* USER CODE BEGIN RNG_Init 1 */

  /* USER CODE END RNG_Init 1 */
  hrng.Instance = RNG;
  hrng.Init.ClockErrorDetection = RNG_CED_ENABLE;
  if (HAL_RNG_Init(&hrng) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN RNG_Init 2 */

  /* USER CODE END RNG_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart1, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart1, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

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
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CS_GPIO_Port, SPI_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(SPI_CHIP_EN_GPIO_Port, SPI_CHIP_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : SPI_SCK_Pin SPI_MISO_Pin SPI_MOSI_Pin */
  GPIO_InitStruct.Pin = SPI_SCK_Pin|SPI_MISO_Pin|SPI_MOSI_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_CS_Pin */
  GPIO_InitStruct.Pin = SPI_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI_CS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_RDY_Pin */
  GPIO_InitStruct.Pin = SPI_RDY_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SPI_RDY_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SPI_CHIP_EN_Pin */
  GPIO_InitStruct.Pin = SPI_CHIP_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(SPI_CHIP_EN_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @param None
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
