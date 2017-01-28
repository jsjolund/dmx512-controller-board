/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include "serial.h"
#include <string.h>

#define RX_BUFFER_MAX 100
#define UART_TIMEOUT 0xff

int rxIndex = 0;
uint8_t rxBuffer = 0;
uint8_t rxString[RX_BUFFER_MAX];

UART_HandleTypeDef *huart;

void SerialInit(UART_HandleTypeDef *huart2) {
	huart = huart2;
	HAL_UART_Receive_DMA(huart, &rxBuffer, sizeof(rxBuffer));
}

void SerialExecute(char *rxString) {
	if (strcmp(rxString, "status") == 0) {
		HAL_UART_Transmit(huart, (uint8_t *) "OK", 2, UART_TIMEOUT);
	} else {
		HAL_UART_Transmit(huart, (uint8_t *) "ERROR", 5, UART_TIMEOUT);
	}
	HAL_UART_Transmit(huart, (uint8_t *) "\r\n", 2, UART_TIMEOUT);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	int i;
	if (rxBuffer == '\r' || rxBuffer == '\n') {
		// Echo carriage return
		HAL_UART_Transmit(huart, (uint8_t *) "\r\n", 2, UART_TIMEOUT);
		rxString[rxIndex] = 0;
		// Execute command
		SerialExecute((char*) rxString);
		// Clear the buffer
		rxIndex = 0;
		for (i = 0; i < RX_BUFFER_MAX; i++)
			rxString[i] = 0;
	} else {
		// Echo the character
		HAL_UART_Transmit(huart, (uint8_t *) &rxBuffer, 1, UART_TIMEOUT);
		// Append character and increment cursor
		rxString[rxIndex] = rxBuffer;
		if (rxIndex < RX_BUFFER_MAX - 1)
			rxIndex++;

	}
}
