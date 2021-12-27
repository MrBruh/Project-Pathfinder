/*
 * gy_88.c
 *
 *  Created on: Dec 26, 2021
 *      Author: Michael Denissov
 */


#include "gy_88.h"

/**
 * @brief  Verifies that there is an MPU6050 connected to the main i2c line and configures
 * it if there is one.
 * @note   None
 * @retval status: the HAL status of all the read and writes to the HAL i2c line.
 */
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

/**
 * @brief  Reads the acceleration values in the x, y, and z dimensions of the accelerometer.
 * @note   Fails if the MPU6050 is not connected or configured properly
 * @param  accel_data[3]: The buffer where the acceleration values x,y,z are written to in that
 * order.
 * @retval status: The HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef MPU6050_Read_Acceleration(int16_t *accel_data)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t mpu6050_buf[6];

	// Get acceleration data from the MPU6050
	status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, ACCEL_XOUT_H_REG,
							  1, &mpu6050_buf, 6, 100);
	if (status != HAL_OK)
		return status;

	// The data is read as upper and lower bytes, so we combine them
	int16_t a_x_raw = (int16_t)(mpu6050_buf[0] << 8 | mpu6050_buf[1]);
	int16_t a_y_raw = (int16_t)(mpu6050_buf[2] << 8 | mpu6050_buf[3]);
	int16_t a_z_raw = (int16_t)(mpu6050_buf[4] << 8 | mpu6050_buf[5]);

	// Write the values to accel_data
	accel_data[0] = a_x_raw/16384.0;
	accel_data[1] = a_y_raw/16384.0;
	accel_data[2] = a_z_raw/16384.0;

	return status;
}

/**
 * @brief  Reads the gyro values in the x, y, and z dimensions of the gyroscope.
 * @note   Fails if the MPU6050 is not connected or configured properly
 * @param  gyro_data[3]: The buffer where the gyro values x,y,z are written to in that order.
 * @retval status: The HAL status of all the read and writes to the HAL i2c line.
 */
HAL_StatusTypeDef MPU6050_Read_Gyro(int16_t *gyro_data)
{
	HAL_StatusTypeDef status = HAL_OK;
	uint8_t mpu6050_buf[6];

	// Get gyroscopic data from the MPU6050
	// The registers XOUT_H to ZOUT_L are all next to each other, so we can just read 6
	// consecutive bytes starting from XOUT_H
	status = HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, GYRO_XOUT_H_REG,
							  1, &mpu6050_buf, 6, 100);
	if (status != HAL_OK)
		return status;

	// The data is read as upper and lower bytes, so we combine them
	int16_t g_x_raw = (int16_t)(mpu6050_buf[0] << 8 | mpu6050_buf[1]);
	int16_t g_y_raw = (int16_t)(mpu6050_buf[2] << 8 | mpu6050_buf[3]);
	int16_t g_z_raw = (int16_t)(mpu6050_buf[4] << 8 | mpu6050_buf[5]);

	// Write the values to gyro_data
	gyro_data[0] = g_x_raw/131.0;
	gyro_data[1] = g_y_raw/131.0;
	gyro_data[2] = g_z_raw/131.0;

	return status;
}
