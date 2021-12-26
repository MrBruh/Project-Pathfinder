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

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartStatusTask(void *argument);
void StartCommsTask(void *argument);

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
	HAL_StatusTypeDef status = MPU6050_Init();
	if (status != HAL_OK)
	{
		char buffer[40];
		sprintf(buffer, "gy-88 init failed\n\rs: %02x\n\r", status);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	}

	int gyro_data[3];
	for(;;)
	{
		osDelay(1000);
		status = MPU6050_Read_Gyro(gyro_data);
		if (status != HAL_OK)
		{
			char buffer[50];
			sprintf(buffer, "reading from gyro failed\n\rs: %02x\n\r", status);
			HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
			continue;
		}
		// Print gyro data if successful
		{
		char buffer[50];
		sprintf(buffer, "g_x: %d, g_y: %d, g_z: %d\n\r", gyro_data[0], gyro_data[1], gyro_data[2]);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
		}

		status = MPU6050_Read_Acceleration(gyro_data);
		if (status != HAL_OK)
		{
			char buffer[50];
			sprintf(buffer, "reading from accel failed\n\rs: %02x\n\r", status);
			HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
			continue;
		}
		// Print gyro data if successful
		{
		char buffer[50];
		sprintf(buffer, "a_x: %d, a_y: %d, a_z: %d\n\r", gyro_data[0], gyro_data[1], gyro_data[2]);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
		}

	}
	/* USER CODE END StartCommsTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

