/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include <string.h>
#include "serial.h"

#define RX_BUFFER_MAX 100
#define TX_BUFFER_MAX 100

int rxIndex = 0;
uint8_t rxBuffer = 0;
uint8_t rxString[RX_BUFFER_MAX];

uint8_t txBuffer = 0;
uint8_t txString[TX_BUFFER_MAX];
int txIn, txOut;

UART_HandleTypeDef *huart;

int QueuePut(uint8_t new) {
	if (txIn == ((txOut - 1 + TX_BUFFER_MAX) % TX_BUFFER_MAX))
		return 0;
	txString[txIn] = new;
	txIn = (txIn + 1) % TX_BUFFER_MAX;
	return 1;
}

int QueueGet(uint8_t *old) {
	if (txIn == txOut)
		return 0;
	*old = txString[txOut];
	txOut = (txOut + 1) % TX_BUFFER_MAX;
	return 1;
}

void SerialInit(UART_HandleTypeDef *huart2) {
	huart = huart2;
	txIn = txOut = 0;

	HAL_NVIC_EnableIRQ(USART2_IRQn);
	HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);

	HAL_UART_Receive_DMA(huart, &rxBuffer, sizeof(rxBuffer));
}

void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(huart);
	__HAL_UART_ENABLE_IT(huart, UART_IT_TC);
}

void SerialTransmit(char *ptr, int len) {
	HAL_UART_Transmit(huart, (uint8_t *) ptr, len, 100);

//	int i = 0;
//	for (i = 0; i < len; i++)
//		QueuePut(*ptr);
//
//	if (USART_SR_TXE & USART2->SR) {
//		uint8_t c;
//		QueueGet(&c);
//		HAL_UART_Transmit_DMA(huart, &c, 1, 100);
//	}
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (USART_SR_TXE & USART2->SR) {
//		HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
//		HAL_UART_IRQHandler(huart);
//		HAL_UART_Transmit_DMA(huart, (uint8_t *) "a", 1);
	}
}

void SerialExecute() {
	HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	int i;
	if (rxBuffer == '\r' || rxBuffer == '\n') {
		// Echo carriage return
		HAL_UART_IRQHandler(huart);
		HAL_UART_Transmit_DMA(huart, (uint8_t *) "\r\n", 2);

		rxString[rxIndex] = 0;
		// FIXME: Send a command to DMX line?
		SerialExecute();

		// Clear the buffer
		rxIndex = 0;
		for (i = 0; i < RX_BUFFER_MAX; i++)
			rxString[i] = 0;
	} else {
		// Echo the character
		HAL_UART_IRQHandler(huart);
		HAL_UART_Transmit_DMA(huart, (uint8_t *) &rxBuffer, 1);

		// Append character and increment cursor
		rxString[rxIndex] = rxBuffer;
		if (rxIndex < RX_BUFFER_MAX - 1)
			rxIndex++;

	}
}
