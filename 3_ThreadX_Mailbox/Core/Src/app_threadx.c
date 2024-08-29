/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    app_threadx.c
  * @author  MCD Application Team
  * @brief   ThreadX applicative file
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
#include "app_threadx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "main.h"
#include "stdio.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define THREAD_STACK_SIZE 1024
#define TEMP_THREAD_STACK_SIZE 1024
#define UART_THREAD_STACK_SIZE 1024
#define QUEUE_STACK_SIZE 256

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */
uint8_t	thread_stack1[THREAD_STACK_SIZE];
TX_THREAD thread_ptr1;
uint8_t	thread_stack2[THREAD_STACK_SIZE];
TX_THREAD thread_ptr2;
uint8_t	thread_stack3[THREAD_STACK_SIZE];
TX_THREAD thread_ptr3;

TX_SEMAPHORE semaphore_0;

TX_THREAD temp_thread_ptr;
TX_THREAD uart_thread_ptr;
TX_QUEUE temp_queue;
uint8_t temp_thread_stack[TEMP_THREAD_STACK_SIZE];
uint8_t uart_thread_stack[UART_THREAD_STACK_SIZE];
ULONG queue_stack[QUEUE_STACK_SIZE];


extern	UART_HandleTypeDef huart3;				// Declared in main.c
extern	DTS_HandleTypeDef hdts;					// Declared in main.c


/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */
VOID LD1_thread_entry(ULONG initial_input);
VOID LD2_thread_entry(ULONG initial_input);
VOID LD3_thread_entry(ULONG initial_input);

VOID temp_thread_entry(ULONG initial_input);
VOID uart_thread_entry(ULONG initial_input);


/* USER CODE END PFP */

/**
  * @brief  Application ThreadX Initialization.
  * @param memory_ptr: memory pointer
  * @retval int
  */
UINT App_ThreadX_Init(VOID *memory_ptr)
{
  UINT ret = TX_SUCCESS;
  /* USER CODE BEGIN App_ThreadX_MEM_POOL */

  /* USER CODE END App_ThreadX_MEM_POOL */
  /* USER CODE BEGIN App_ThreadX_Init */

  tx_thread_create(&thread_ptr1, "LD1_thread", LD1_thread_entry, 0x1234, thread_stack1, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  tx_thread_create(&thread_ptr2, "LD2_thread", LD2_thread_entry, 0x1234, thread_stack2, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  tx_thread_create(&thread_ptr3, "LD3_thread", LD3_thread_entry, 0x1234, thread_stack3, THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);

  tx_semaphore_create(&semaphore_0,"semaphore_0",0);

  tx_queue_create(&temp_queue,"TEMP Queue", 1, queue_stack, 256);

  tx_thread_create(&temp_thread_ptr, "TEMP Thread", temp_thread_entry, 0, temp_thread_stack, TEMP_THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);
  tx_thread_create(&uart_thread_ptr, "UART Thread", uart_thread_entry, 0, uart_thread_stack, UART_THREAD_STACK_SIZE, 15, 15, 1, TX_AUTO_START);

  /* USER CODE END App_ThreadX_Init */

  return ret;
}

  /**
  * @brief  Function that implements the kernel's initialization.
  * @param  None
  * @retval None
  */
void MX_ThreadX_Init(void)
{
  /* USER CODE BEGIN  Before_Kernel_Start */

  /* USER CODE END  Before_Kernel_Start */

  tx_kernel_enter();

  /* USER CODE BEGIN  Kernel_Start_Error */

  /* USER CODE END  Kernel_Start_Error */
}

/* USER CODE BEGIN 1 */
VOID LD1_thread_entry (ULONG initial_input){
	while(1){
		HAL_GPIO_TogglePin(LD1_GPIO_Port, LD1_Pin);
		tx_thread_sleep(100);
	}

}
VOID LD2_thread_entry (ULONG initial_input){
	while(1){
		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
		tx_thread_sleep(200);
	}

}
VOID LD3_thread_entry (ULONG initial_input){

	ULONG current_value;
	TX_THREAD *first_suspended;
	ULONG suspended_count;
	TX_SEMAPHORE *next_semaphore;

	while(1){

		tx_semaphore_get(&semaphore_0, TX_WAIT_FOREVER);

		printf("User Button Pressed\n\r");
		UINT status = _tx_semaphore_info_get(&semaphore_0,
				NULL,
				&current_value,
				&first_suspended,
				&suspended_count,
				&next_semaphore);

		if (status == TX_SUCCESS){
			printf("Current Value: %lu\n\r\n\n", current_value);
		}

		tx_thread_sleep(50);
		HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);	//Turn of LD3
	}
}


void temp_thread_entry(ULONG thread_input){

	int32_t Temperature;

	while(1){
	if(HAL_DTS_Start(&hdts)!= HAL_OK){
			     /* DTS start Error */
			     Error_Handler();
			   }

		  /* Get temperature in deg C */
		 if(HAL_DTS_GetTemperature(&hdts, &Temperature)!= HAL_OK){
			      /* DTS GetTemperature Error */
			      Error_Handler();
			  }

		  printf("From Temp Thread: %lu\r\n", Temperature);

		   // Send the TEMP value to the UART thread
		        tx_queue_send(&temp_queue, &Temperature, TX_WAIT_FOREVER);

		  if(HAL_DTS_Stop(&hdts) != HAL_OK){
				  Error_Handler();
			  }

        // Delay for a while (e.g., 1 second)
        tx_thread_sleep(100);
	}
}

void uart_thread_entry(ULONG thread_input){
	  LONG receivedValue;


	    while (1) {

	    	// Wait to receive data from the TEMP thread
	        if (tx_queue_receive(&temp_queue, &receivedValue, TX_WAIT_FOREVER) == TX_SUCCESS) {

	        	//printf("From Temp Thread: %lu\r\n", Temperature);
	        	 printf("From UART Thread: %lu\r\n", receivedValue);

	        }
	    }

}




/* USER CODE END 1 */
