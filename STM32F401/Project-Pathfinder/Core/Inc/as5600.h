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

#define AS5600_THRESHOLD_PERCENT 0.1
#define AS5600_MAX_RAW_ANGLE 4096
#define AS5600_SIGNIFICANCE_PERCENT 0

#include "main.h"
#include <stdint.h> /* for int16_t */
#include "stm32f4xx_hal.h" /* for HAL_StatusTypeDef */
#include <stdbool.h>
#include <stdlib.h>

#include "i2c.h"

extern uint16_t encoder_range[];
extern char encoder_debug_values[];
extern int32_t encoder_debug_counter;

void AS5600_Init(TIM_TypeDef *TIM_INPUT);
HAL_StatusTypeDef AS5600_Get_Raw_Angle(uint16_t *raw_angle);
HAL_StatusTypeDef AS5600_Get_Status(uint8_t *as5600_status);
void AS5600_Reset_Encoder_Timer(uint32_t time);
HAL_StatusTypeDef AS5600_Update_Encoder_Range(int cycles);
HAL_StatusTypeDef AS5600_Update_Encoder_Speed(void);
uint8_t AS5600_Is_Encoder_Data_Ready(void);
uint32_t AS5600_Get_Encoder_Speed(void);

#endif /* INC_AS5600_H_ */
