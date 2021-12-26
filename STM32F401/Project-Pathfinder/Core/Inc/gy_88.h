/*
 * gy_88.h
 *
 *  Created on: Dec 26, 2021
 *      Author: Michael Denissov
 */

#ifndef INC_GY_88_H_
#define INC_GY_88_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h> /* for debug messages */
#include <stdio.h> /* for sprintf */
#include <stdint.h> /* for int16_t */
#include "stm32f4xx_hal.h" /* for HAL_StatusTypeDef */

#include "i2c.h"
#include "usart.h"

#define MPU6050_ADDR 0xD0

// Register addresses
#define SMPLRT_DIV_REG 0x19
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C
#define ACCEL_XOUT_H_REG 0x3B
#define TEMP_OUT_H_REG 0x41
#define GYRO_XOUT_H_REG 0x43
#define PWR_MGMT_REG 0x6B
#define WHO_AM_I_REG 0x75

HAL_StatusTypeDef MPU6050_Init(void);
HAL_StatusTypeDef MPU6050_Read_Acceleration(int16_t *accel_data);
HAL_StatusTypeDef MPU6050_Read_Gyro(int16_t *gyro_data);

#ifdef __cplusplus
}
#endif

#endif /* INC_GY_88_H_ */
