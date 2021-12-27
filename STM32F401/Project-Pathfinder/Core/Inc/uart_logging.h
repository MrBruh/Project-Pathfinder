/*
 * uart_logging.h
 *
 *  Created on: Dec. 27, 2021
 *      Author: Michael Denissov
 */

#ifndef INC_UART_LOGGING_H_
#define INC_UART_LOGGING_H_

#ifdef __cplusplus
extern "C" {
#endif

#define UART_LOGGING_STATUS	// For sending HAL status messages
#define UART_LOGGING_DEBUG	// For general debugging purposes

#include <string.h> /* for debug messages */
#include <stdio.h> /* for sprintf */
#include <stdlib.h> /* malloc & free */
#include "stm32f4xx_hal.h" /* for HAL_StatusTypeDef */

#include "usart.h"

// HAL status message
void UART_Log_Status(const char *msg, HAL_StatusTypeDef status);

// General debug messages
void UART_Log_Debug(const char *msg);
void UART_Log_Debug_U8(const char *msg, const uint8_t var);

#ifdef __cplusplus
}
#endif

#endif /* INC_UART_LOGGING_H_ */
