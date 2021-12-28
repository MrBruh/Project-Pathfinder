/*
 * uart_logging.c
 *
 *  Created on: Dec. 27, 2021
 *      Author: Michael Denissov
 */

#include "uart_logging.h"

/**
 * @brief  Sends a status logging message to the UART connection
 * @note   There is no space before the status is appended to the message
 * @param  msg: The logging message to be sent
 * @param  status: The status to be appended to the logging message
 * @retval None
 */
void UART_Log_Status(const char *msg, HAL_StatusTypeDef status)
{
#ifdef UART_LOGGING_STATUS
	char *buffer = malloc(sizeof(char) * strlen(msg) + 10);
	sprintf(buffer, msg);
	sprintf(buffer + strlen(buffer), "%02x\n\r", status);
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	free(buffer);
#endif
}

/**
 * @brief  Sends a general logging message to the UART connection
 * @note   None
 * @param  msg: The logging message to be sent
 * @retval None
 */
void UART_Log_Debug(const char *msg)
{
#ifdef UART_LOGGING_DEBUG
	HAL_UART_Transmit(&huart1, msg, strlen(msg), 100);
#endif
}

/**
 * @brief  Sends a general logging message to the UART connection
 * @note   There is no space before the variable is appended to the message
 * @param  msg: The logging message to be sent
 * @param  var: The uint8_t variable to be appended to the logging message
 * @retval None
 */
void UART_Log_Debug_U8(const char *msg, const uint8_t var)
{
#ifdef UART_LOGGING_STATUS
	char *buffer = malloc(sizeof(char) * strlen(msg) + 10);
	sprintf(buffer, msg);
	sprintf(buffer + strlen(buffer), "%02x\n\r", var);
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	free(buffer);
#endif
}

void UART_Log_Debug_U32(const char *msg, const int32_t var)
{
#ifdef UART_LOGGING_STATUS
	char *buffer = malloc(sizeof(char) * strlen(msg) + 50);
	sprintf(buffer, msg);
	sprintf(buffer + strlen(buffer), "%d", var);
	HAL_UART_Transmit(&huart1, buffer, strlen(buffer), 100);
	free(buffer);
#endif
}

