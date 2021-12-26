/*
 * gy_88.c
 *
 *  Created on: Dec 26, 2021
 *      Author: Michael Denissov
 */


#include "gy_88.h"

HAL_StatusTypeDef MPU6050_Init(void)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t _check, _data;

	// Check the MPU6050's ID (Should be 0x68)
	status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &_check, 1, 100);
	if (status != HAL_OK)
		return status;

	if (_check == 0x68)
	{
		// Activate the sensor by setting power management to 0
		_data = 0;
		status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_REG, 1, &_data, 1, 100);
		if (status != HAL_OK)
			return status;

		// 1khz sampling rate
		_data = 0x07;
		status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &_data, 1, 100);
		if (status != HAL_OK)
			return status;

		// Set default sensor configurations
		_data = 0;
		status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &_data, 1, 100);
		if (status != HAL_OK)
			return status;

		status = HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &_data, 1, 100);
		if (status != HAL_OK)
			return status;

	}
	else
	{
		// Create and send debug message
		char buffer[40];
		sprintf(buffer, "gy-88 mem check failed\n\rc: %02x\n\r", _check);
		HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	}

	return status;
}


HAL_StatusTypeDef MPU6050_Read_Acceleration(int16_t *accel_data)
{
	HAL_StatusTypeDef status = HAL_OK;
	return status;
}
HAL_StatusTypeDef MPU6050_Read_Gyro(int16_t *gyro_data)
{
	HAL_StatusTypeDef status = HAL_OK;
	return status;
}
