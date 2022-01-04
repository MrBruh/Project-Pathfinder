/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2021 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "i2c.h"
#include "usart.h"
#include "gy_88.h"
#include "as5600.h"

#include <string.h> /* for debug messages */
#include <stdio.h> /* for sprintf */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
int gyro_started = 0;
int encoder_started = 0;
/* USER CODE END Variables */
/* Definitions for statusTask */
osThreadId_t statusTaskHandle;
const osThreadAttr_t statusTask_attributes = {
  .name = "statusTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};
/* Definitions for commsTask */
osThreadId_t commsTaskHandle;
const osThreadAttr_t commsTask_attributes = {
  .name = "commsTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readSensorGyro */
osThreadId_t readSensorGyroHandle;
const osThreadAttr_t readSensorGyro_attributes = {
  .name = "readSensorGyro",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartStatusTask(void *argument);
void StartCommsTask(void *argument);
void StartReadSensorGyro(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of statusTask */
  statusTaskHandle = osThreadNew(StartStatusTask, NULL, &statusTask_attributes);

  /* creation of commsTask */
  commsTaskHandle = osThreadNew(StartCommsTask, NULL, &commsTask_attributes);

  /* creation of readSensorGyro */
  readSensorGyroHandle = osThreadNew(StartReadSensorGyro, NULL, &readSensorGyro_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
	/* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartStatusTask */
/**
 * @brief  Function implementing the statusTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartStatusTask */
void StartStatusTask(void *argument)
{
  /* USER CODE BEGIN StartStatusTask */
	// Currently only blink every second to indicate the program is running
	for(;;)
	{
		osDelay(1000);
		GPIOC -> ODR ^= GPIO_PIN_13;
	}
  /* USER CODE END StartStatusTask */
}

/* USER CODE BEGIN Header_StartCommsTask */
/**
 * @brief Function implementing the commsTask thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartCommsTask */
void StartCommsTask(void *argument)
{
  /* USER CODE BEGIN StartCommsTask */

	// Delay to allow time for hardware to power up
	osDelay(100);

	// Start the PWM generation at 7% for calibration
	int speed = 65535 * 0.07;
	TIM2->CCR1 = speed;

	// Begin the AS5600 encoder
	UART_Log_Debug("encoder init\n\r");
	AS5600_Init(TIM5);
	AS5600_Reset_Encoder_Timer(TIM5->CNT);
	HAL_StatusTypeDef status = AS5600_Update_Encoder_Range(2000);
	if (status != HAL_OK)
	{
		/*while (status != HAL_OK)
		{
			osDelay(1000);
			UART_Log_Status("encoder init failed\n\rs: ", status);
			status = AS5600_Update_Encoder_Range(2000);
		}*/
		UART_Log_Status("encoder init failed\n\rs: ", status);
	}
	else
	{
		encoder_started = 1;
		// Set the PWM generation at 20% for testing speed
		speed = 65535 * 0.1;
		TIM2->CCR1 = speed;
	}

	/*for (int i = 0; i < 500; i++)
	{
		UART_Log_Debug_32("e_value: ", encoder_debug_values[i]);
		UART_Log_Debug("\n\r");
	}*/

	UART_Log_Debug("gy-88 init\n\r");
	status = MPU6050_Init();
	if (status != HAL_OK)
		UART_Log_Status("gy-88 init failed\n\rs: ", status);
	else
		gyro_started = 1;

	for(;;)
	{
		osDelay(1000);

		// Print debug information
		if (gyro_started == 1)
		{
			UART_Log_Debug_32("g_x: ", (int32_t)(gyro_pos.x) /754);
			UART_Log_Debug_32(" g_y: ", (int32_t)(gyro_pos.y) /754);
			UART_Log_Debug_32(" g_z: ", (int32_t)(gyro_pos.z) /754 );
		}

		if (encoder_started == 1)
		{
			UART_Log_Debug_32(" encoder min: ", encoder_range[0]);
			UART_Log_Debug_32(" encoder max: ", encoder_range[1]);
			UART_Log_Debug_32(" e: ", encoder_debug_counter);
			if (AS5600_Is_Encoder_Data_Ready() == 1)
				UART_Log_Debug_32(" encoder speed: ", AS5600_Get_Encoder_Speed());
			if (encoder_debug_counter < 1)
			{
				UART_Log_Debug(encoder_debug_values);
				encoder_debug_counter = 1;
				encoder_debug_values[0] = '\0';
			}
		}

		UART_Log_Debug("\n\r");
	}
  /* USER CODE END StartCommsTask */
}

/* USER CODE BEGIN Header_StartReadSensorGyro */
/**
 * @brief Function implementing the readSensorGyro thread.
 * @param argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartReadSensorGyro */
void StartReadSensorGyro(void *argument)
{
  /* USER CODE BEGIN StartReadSensorGyro */
	osDelay(1000);

	// Begin the timer for the gyro readings
	MPU6050_Reset_Gyro_Timer(TIM5->CNT);
	// Update the gyro readings every 10 milliseconds
	for(;;)
	{
		if (gyro_started == 1)
			MPU6050_Update_Gyro_Pos();
		if (encoder_started == 1)
			AS5600_Update_Encoder_Speed();
		osDelay(10);
	}
  /* USER CODE END StartReadSensorGyro */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

