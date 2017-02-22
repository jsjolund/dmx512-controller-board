/*
 * serial.cpp
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "serial.h"
#include "dmx512.h"

#define RX_BUFFER_MAX 20
#define TX_BUFFER_MAX 40

static int usbRxIndex = 0;
static uint8_t usbRxBuffer = 0;
static uint8_t usbRxString[RX_BUFFER_MAX];

static uint8_t usbTxString[TX_BUFFER_MAX];
static int usbTxIndex, usbTxOutdex;

static UART_HandleTypeDef *usbHuart;

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
	HAL_NVIC_SetPriority(USB_USART_IRQ, 0, 0);

	HAL_UART_Receive_DMA(usbHuart, &usbRxBuffer, sizeof(usbRxBuffer));
}

int isDigit(char c) {
	return ('0' <= c && c <= '9');
}

int isLetter(char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || (c == ' ');
}

void SerialExecute(char* string) {
	// Parse a user command
	uint8_t fail = 0;
	char *cmd = "set ";
	if (strncmp(string, cmd, strlen(cmd)) == 0 && strlen(string) > strlen(cmd)) {
		int iStr, iChn, iVal;
		iChn = iVal = 0;
		char strChn[4] = { '\0', '\0', '\0', '\0' };
		char strVal[4] = { '\0', '\0', '\0', '\0' };

		for (iStr = strlen(cmd); iStr < strlen(string); iStr++) {
			char c = string[iStr];
			if (isDigit(c)) {
				if (iChn >= 0 && iChn < 3) {
					strChn[iChn] = c;
					iChn++;
				} else if (iChn == -1 && iVal < 3) {
					strVal[iVal] = c;
					iVal++;
				} else {
					fail = 1;
				}
			} else if (c == ' ') {
				iChn = -1;
			} else {
				fail = 1;
				break;
			}
		}
		if (iChn == 0 || iVal == 0)
			fail = 1;
		if (!fail) {
			int channel = atoi(strChn);
			int value = atoi(strVal);
			if (channel < 512 && value < 256) {
				printf("Setting channel %d to %d\r\n", channel, value);
				Dmx512SetChannelValue(channel, value);
			} else {
				printf("Maximum values exceeded\r\n");
			}
		}
	} else {
		fail = 1;
	}
	if (fail) {
		printf("ERROR, could not parse %s\r\n", string);
	}

}

void SerialTransmit(char *ptr, int len) {
	int i = 0;
	for (i = 0; i < len; i++)
		SerialQueuePut(ptr[i]);

	if (USART_SR_TC & USB_USART->SR) {
		uint8_t c;
		SerialQueueGet(&c);
		while (HAL_UART_Transmit_DMA(usbHuart, &c, 1) != HAL_OK)
			;
		HAL_UART_IRQHandler(usbHuart);
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huartHandle) {

	if (huartHandle == usbHuart) {
		int i;
		uint8_t usbRxChar = usbRxBuffer;
		if (usbRxChar == 127 || usbRxChar == 8) {
			// Backspace or delete
			while (HAL_UART_Transmit_DMA(usbHuart, &usbRxChar, 1) != HAL_OK)
				;
			usbRxIndex = (usbRxIndex > 0) ? usbRxIndex - 1 : 0;
			usbRxString[usbRxIndex] = 0;

		} else if (usbRxChar == '\r' || usbRxChar == '\n') {
			// Echo carriage return
			while (HAL_UART_Transmit_DMA(usbHuart, (uint8_t *) "\r\n", 2) != HAL_OK)
				;
			// Add null terminator
			usbRxString[usbRxIndex] = '\0';
			// TODO: Send a command to DMX line?
			SerialExecute((char *) &usbRxString);
			// Clear the buffer
			usbRxIndex = 0;
			for (i = 0; i < RX_BUFFER_MAX; i++)
				usbRxString[i] = 0;

		} else if (isDigit(usbRxChar) || isLetter(usbRxChar)) {
			// Echo the character
			while (HAL_UART_Transmit_DMA(usbHuart, &usbRxChar, 1) != HAL_OK)
				;
			// Append character and increment cursor
			usbRxString[usbRxIndex] = usbRxChar;
			if (usbRxIndex < RX_BUFFER_MAX - 1)
				usbRxIndex++;
		}
	}
}

void USART2_IRQHandler(void) {
	HAL_UART_IRQHandler(usbHuart);

	if (USART_SR_TXE & USB_USART->SR) {
		uint8_t c;
		if (SerialQueueGet(&c)) {
			HAL_UART_Transmit_DMA(usbHuart, &c, 1);
			HAL_UART_IRQHandler(usbHuart);
		}
	}
}
