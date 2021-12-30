/*
 * as5600.h
 *
 *  Created on: Dec. 28, 2021
 *      Author: Michael Denissov
 */

#ifndef INC_AS5600_H_
#define INC_AS5600_H_

#define AS5600_ADDRESS 0x36 << 1
#define AS5600_REGISTER_RAW_ANGLE 0x0C
#define AS5600_REGISTER_STATUS 0x0B

#include "main.h"
#include <stdint.h> /* for int16_t */
#include "stm32f4xx_hal.h" /* for HAL_StatusTypeDef */

#include "i2c.h"

extern uint16_t encoder_range[2];

void AS5600_Init(void);
HAL_StatusTypeDef AS5600_Get_Raw_Angle(uint16_t *raw_angle);
HAL_StatusTypeDef AS5600_Get_Status(uint8_t *as5600_status);
void AS5600_Reset_Encoder_Timer(uint32_t time);
HAL_StatusTypeDef AS5600_Update_Encoder_Range(int cycles);

#endif /* INC_AS5600_H_ */
