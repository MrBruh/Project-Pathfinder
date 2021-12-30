/*
 * as5600.c
 *
 *  Created on: Dec. 28, 2021
 *      Author: Michael Denissov
 */


#include "as5600.h"

uint32_t last_timer_count;

uint16_t encoder_range[2];
uint16_t encoder_debug_values[500];
uint16_t encoder_delta = 0;

uint16_t encoder_previous_angle;

enum encoder_state_enum
{
	AS5600_UNINITIALIZED,
	AS5600_INITIALIZED,
	AS5600_CALIBRATED
} encoder_state = AS5600_UNINITIALIZED;

/**
 * @brief  None
 * @note   None
 * @retval None
 */
void AS5600_Init(void)
{
	encoder_range[0] = -1;// Largest uint value
	encoder_range[1] = 0;	// Smallest uint value
	encoder_state = AS5600_INITIALIZED;

	AS5600_Get_Raw_Angle(&encoder_previous_angle);
}

/**
 * @brief  None
 * @note   None
 * @retval status: the HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef AS5600_Get_Raw_Angle(uint16_t *raw_angle)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint32_t current_timer_count = TIM5->CNT;

	// Read from the encoder every 500 microseconds
	if (current_timer_count - last_timer_count >= TIM5_COUNTS_PER_MICROSECOND * 500)
	{
		// Read high and low bytes
		uint8_t as5600_buf[2];
		status = HAL_I2C_Mem_Read(&hi2c1, AS5600_ADDRESS, AS5600_REGISTER_RAW_ANGLE,
				1, as5600_buf, 2, 100);
		if (status != HAL_OK)
			return status;

		// Combine high and low
		*raw_angle = (int16_t)(as5600_buf[0] << 8 | as5600_buf[1]);
	}
	else
		status = 4;

	return status;
}

/**
 * @brief  None
 * @note   None
 * @retval status: the HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef AS5600_Get_Status(uint8_t *as5600_status)
{
	HAL_StatusTypeDef status = HAL_OK;
	return status;
}

/**
 * @brief  Resets the encoder timer used for timing reads
 * @retval None
 */
void AS5600_Reset_Encoder_Timer(uint32_t time)
{
	last_timer_count = time;
}

/**
 * @brief  None
 * @retval status: the HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef AS5600_Update_Encoder_Range(int cycles)
{
	uint16_t encoder_data;
	HAL_StatusTypeDef status = HAL_OK;

	int i = 0;
	// Update the encoder range cycles amount of times
	for (int current_cycles = 0; current_cycles < cycles; current_cycles++)
	{
		// get the current angle
		for(;;)
		{
			// Keep trying to read until success or failure
			status = AS5600_Get_Raw_Angle(&encoder_data);
			if (status == 4)
				continue;
			else if (status != HAL_OK)
				return status;
			else
				break;
		}
		// Update the encoder range if necessary
		if (encoder_data > encoder_range[1])
			encoder_range[1] = encoder_data;
		else if (encoder_data < encoder_range[0])
			encoder_range[0] = encoder_data;

		// Add it to the debug list
		encoder_debug_values[i] = encoder_data;
		i++;
	}

	encoder_state = AS5600_CALIBRATED;

	return status;
}
