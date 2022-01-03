/*
 * as5600.c
 *
 *  Created on: Dec. 28, 2021
 *      Author: Michael Denissov
 */


#include "as5600.h"

#include <string.h>
#include <stdio.h>
void AS5600_Debug_Print(const char *msg, const int32_t var);

TIM_TypeDef *TIMER;
uint32_t last_timer_count;

uint16_t encoder_range[2];
uint16_t encoder_significance_value = 0;
char encoder_debug_values[1000];
int8_t encoder_debug_counter = 1;
uint32_t encoder_delta = 0;		// Difference in timer between rotations
uint8_t encoder_data_ready = 0;	// 1 when encoder_delta has updated

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
void AS5600_Init(TIM_TypeDef *TIM_INPUT)
{
	encoder_range[0] = -1;// Largest uint value
	encoder_range[1] = 0;	// Smallest uint value
	encoder_state = AS5600_INITIALIZED;

	AS5600_Get_Raw_Angle(&encoder_previous_angle);

	TIMER = TIM_INPUT;
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

	//int i = 0;
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
		if (encoder_data < encoder_range[0])
			encoder_range[0] = encoder_data;
		else if (encoder_data > encoder_range[1])
			encoder_range[1] = encoder_data;

		// Add it to the debug list
		//encoder_debug_values[i] = encoder_data;
		//i++;
	}

	// Increase the threshold of the range by a certain percent
	encoder_range[0] += encoder_range[0] * AS5600_THRESHOLD_PERCENT;
	encoder_range[1] -= encoder_range[1] * AS5600_THRESHOLD_PERCENT;

	encoder_significance_value = encoder_range[1] - encoder_range[0] * AS5600_SIGNIFICANCE_PERCENT;

	encoder_state = AS5600_CALIBRATED;

	return status;
}

/**
 * @brief  None
 * @retval status: the HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef AS5600_Update_Encoder_Speed()
{
	// Timing information
	static bool timer_started = false;
	static uint32_t prev_time = 0;
	uint32_t current_time = TIMER->CNT;

	// Direction is either positive or negative, 0 is uninitialized
	static int8_t save_point_direction = 0;
	static int8_t prev_direction = 0;
	int8_t current_direction = 0;

	// Current position of the motor
	static uint16_t prev_angle = AS5600_MAX_RAW_ANGLE + 1;
	uint16_t current_angle;

	HAL_StatusTypeDef status = AS5600_Get_Raw_Angle(&current_angle);
	if (status != HAL_OK)
	{
		AS5600_Debug_Print("!OK", current_angle);
		return status;
	}


	if (prev_angle > AS5600_MAX_RAW_ANGLE)
		prev_angle = current_angle;

	// Only proceed with calculations if there has been a significant change in data
	/*if (abs(current_angle - prev_angle) < encoder_significance_value)
	{
		AS5600_Debug_Print("insig", current_angle);
		AS5600_Debug_Print("prev", prev_angle);
		return status;
	}*/

	// Get the direction of angle change
	if (current_angle - prev_angle > 0)
		current_direction = 1;
	else if (current_angle - prev_angle < 0)
		current_direction = -1;
	else
		current_direction = 0;

	if (prev_direction == 0)
		prev_direction = current_direction;

	// We wait until the encoder value changes direction
	if (current_direction == prev_direction)
	{
		AS5600_Debug_Print("=dir", current_angle);
		return status;
	}

	// Only save the time when we switch directions for the first time entering a threshold
	if ((current_angle < encoder_range[0] || current_angle > encoder_range[1]) &&
		current_direction != save_point_direction)
	{
		if (timer_started == false)
		{
			prev_time = current_time;
			timer_started = true;
			AS5600_Debug_Print("p", current_angle);
		}
		else
		{
			encoder_delta = current_time - prev_time;
			encoder_data_ready = 1;
			AS5600_Debug_Print("n", current_angle);
		}
		save_point_direction = current_direction;
	}

	AS5600_Debug_Print("end", current_angle);
	return status;
}

/**
 * @brief  None
 * @retval
 */
uint8_t AS5600_Is_Encoder_Data_Ready()
{
	return encoder_data_ready;
}

/**
 * @brief  None
 * @retval
 */
uint32_t AS5600_Get_Encoder_Speed()
{
	encoder_data_ready = 0;
	return encoder_delta;
}

void AS5600_Debug_Print(const char *msg, const int32_t var)
{
	if (encoder_debug_counter + strlen(msg) + 10 >= 1000 || encoder_debug_counter < 1)
		return;

	char buf[50];
	sprintf(buf, msg);
	sprintf(buf + strlen(buf), "%d", var);
	sprintf(buf + strlen(buf), "\n\r");
	int len = strlen(buf);

	if (len + encoder_debug_counter < 1000)
		sprintf(encoder_debug_values + strlen(encoder_debug_values), buf);
	else
		encoder_debug_counter = -1;
}
