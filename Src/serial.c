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

uint8_t txComplete;
int usbRxIndex = 0;
uint8_t usbRxBuffer = 0;
uint8_t usbRxString[RX_BUFFER_MAX];

uint8_t usbTxString[TX_BUFFER_MAX];
int usbTxIndex, usbTxOutdex;

UART_HandleTypeDef *usbHuart;

int SerialQueuePut(uint8_t new) {
	if (usbTxIndex == ((usbTxOutdex - 1 + TX_BUFFER_MAX) % TX_BUFFER_MAX))
		return 0;
	usbTxString[usbTxIndex] = new;
	usbTxIndex = (usbTxIndex + 1) % TX_BUFFER_MAX;
	return 1;
}

int SerialQueueGet(uint8_t *old) {
	if (usbTxIndex == usbTxOutdex)
		return 0;
	*old = usbTxString[usbTxOutdex];
	usbTxOutdex = (usbTxOutdex + 1) % TX_BUFFER_MAX;
	return 1;
}

void SerialInit(UART_HandleTypeDef *huart2) {
	usbHuart = huart2;
	usbTxIndex = usbTxOutdex = 0;

	HAL_NVIC_EnableIRQ(USB_USART_IRQ);
	HAL_NVIC_SetPriority(USB_USART_IRQ, 0, 0);

	HAL_UART_Receive_DMA(usbHuart, &usbRxBuffer, sizeof(usbRxBuffer));
}

void SerialExecute(void) {
	printf("OK\r\n");
}

void SerialTransmit(char *ptr, int len) {
	int i = 0;
	for (i = 0; i < len; i++)
		SerialQueuePut(ptr[i]);

	if (USART_SR_TC & USART2->SR) {
		uint8_t c;
		SerialQueueGet(&c);
		HAL_UART_Transmit_DMA(usbHuart, &c, 1);
		HAL_UART_IRQHandler(usbHuart);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart == usbHuart) {
		int i;
		if (usbRxBuffer == '\r' || usbRxBuffer == '\n') {
			// Echo carriage return
			HAL_UART_Transmit_DMA(usbHuart, (uint8_t *) "\r\n", 2);
			// Add null terminator
			usbRxString[usbRxIndex] = 0;
			// TODO: Send a command to DMX line?
			SerialExecute();
			// Clear the buffer
			usbRxIndex = 0;
			for (i = 0; i < RX_BUFFER_MAX; i++)
				usbRxString[i] = 0;
		} else {
			// Echo the character
			HAL_UART_Transmit_DMA(usbHuart, (uint8_t *) &usbRxBuffer, 1);
			// Append character and increment cursor
			usbRxString[usbRxIndex] = usbRxBuffer;
			if (usbRxIndex < RX_BUFFER_MAX - 1)
				usbRxIndex++;
		}
	}
}

void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(usbHuart);

	if (USART_SR_TXE & USART2->SR) {
		uint8_t c;
		if (SerialQueueGet(&c)) {
			HAL_UART_Transmit_DMA(usbHuart, &c, 1);
			HAL_UART_IRQHandler(usbHuart);
		}
	}
}
