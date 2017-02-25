/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include "serial.h"

#define RX_BUFFER_MAX 20
#define TX_BUFFER_MAX 60

static volatile int usbRxIndex = 0;
static uint8_t usbRxBuffer = 0;
static volatile uint8_t usbRxString[RX_BUFFER_MAX];

static volatile uint8_t usbTxString[TX_BUFFER_MAX];
static volatile int usbTxIndex, usbTxOutdex;

static UART_HandleTypeDef *usbHuart;

int isDigit(char c) {
	return ('0' <= c && c <= '9');
}

int isLetter(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == ' ');
}

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

void SerialInit(UART_HandleTypeDef *huartHandle) {
	usbHuart = huartHandle;
	usbTxIndex = usbTxOutdex = 0;

	HAL_NVIC_EnableIRQ(USB_USART_IRQ);
	HAL_NVIC_SetPriority(USB_USART_IRQ, 15, 0);

	HAL_UART_Receive_DMA(usbHuart, &usbRxBuffer, sizeof(usbRxBuffer));
}

void SerialSendNextByte(void) {
	HAL_UART_StateTypeDef uartState = HAL_UART_GetState(usbHuart);
	if ((uartState == HAL_UART_STATE_READY) || (uartState == HAL_UART_STATE_BUSY_RX)) {
		uint8_t c;
		if (SerialQueueGet(&c))
			while (HAL_UART_Transmit_DMA(usbHuart, &c, 1) != HAL_OK)
				;
	}
}

void SerialTransmit(char *ptr, int len) {
	int i = 0;
	for (i = 0; i < len; i++)
		SerialQueuePut(ptr[i]);
	SerialSendNextByte();
}

void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(usbHuart);
	SerialSendNextByte();
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle) {
	if (huartHandle == usbHuart) {
		int i;
		char usbRxChar = (char) usbRxBuffer;
		if (usbRxChar == 127 || usbRxChar == 8) {
			// Backspace or delete
			SerialTransmit(&usbRxChar, 1);
			usbRxIndex = (usbRxIndex > 0) ? usbRxIndex - 1 : 0;
			usbRxString[usbRxIndex] = 0;

		} else if (usbRxChar == '\r' || usbRxChar == '\n') {
			// Echo carriage return
			SerialTransmit("\r\n", 2);
			// Add null terminator
			usbRxString[usbRxIndex] = '\0';
			ShellExecute((char *) &usbRxString);
			// Clear the buffer
			usbRxIndex = 0;
			for (i = 0; i < RX_BUFFER_MAX; i++)
				usbRxString[i] = 0;

		} else if (isDigit(usbRxChar) || isLetter(usbRxChar)) {
			// Echo the character
			SerialTransmit(&usbRxChar, 1);
			// Append character and increment cursor
			usbRxString[usbRxIndex] = usbRxChar;
			if (usbRxIndex < RX_BUFFER_MAX - 1)
				usbRxIndex++;
		}
	}
}
