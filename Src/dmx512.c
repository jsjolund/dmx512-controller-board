/*
 * dmx512.c
 *
 *  Created on: Feb 16, 2017
 *      Author: johsjl-1
 */
#include "dmx512.h"

uint16_t dmxCurrentChannel = 0;
uint8_t dmxData[DMX_CHANNELS];
uint8_t dmxSendState;

TIM_HandleTypeDef *dmxHtim;
UART_HandleTypeDef *dmxHuart;

GPIO_InitTypeDef dmx_GPIO_InitStruct;

int Dmx512SetChannelValue(uint16_t channel, uint8_t value) {
	if (channel >= DMX_CHANNELS)
		return 0;
	dmxData[channel] = value;
	return 1;
}

void Dmx512DisableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_DisableIRQ(DMX512_USART_IRQ);

	dmx_GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	dmx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	dmx_GPIO_InitStruct.Pull = GPIO_NOPULL;
	dmx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &dmx_GPIO_InitStruct);
}

void Dmx512EnableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_EnableIRQ(DMX512_USART_IRQ);

	dmx_GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	dmx_GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	dmx_GPIO_InitStruct.Pull = GPIO_PULLUP;
	dmx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	dmx_GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &dmx_GPIO_InitStruct);
}

void Dmx512Init(TIM_HandleTypeDef *htim2, UART_HandleTypeDef *huart1) {
	dmxHtim = htim2;
	dmxHuart = huart1;

	int i;
	for (i = 0; i < DMX_CHANNELS; i++) {
		dmxData[i] = (i + 1) % 256;
	}

	dmxSendState = STATE_BREAK;
	dmxHtim->Init.Period = MARK_BREAK;

	HAL_NVIC_EnableIRQ(DMX512_USART_IRQ);
	HAL_NVIC_SetPriority(DMX512_USART_IRQ, 0, 0);

	Dmx512DisableUart(dmxHuart);

	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);

	HAL_NVIC_SetPriority(DMX512_TIM_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(DMX512_TIM_IRQ);

	HAL_TIM_Base_Init(dmxHtim);
	HAL_TIM_Base_Start_IT(dmxHtim);
}

void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(dmxHuart);

	if (USART_SR_TXE & USART1->SR) {
		if (dmxCurrentChannel < DMX_CHANNELS) {
			// Send a byte using DMA.
			uint8_t d = dmxData[dmxCurrentChannel];
			HAL_UART_Transmit_DMA(dmxHuart, &d, 1);
			dmxCurrentChannel++;

		} else {
			// Finished sending data over USART. Next is Mark Before Break. Set TX high.
			// Account for the time it takes to setup GPIO output mode.
			dmxHtim->Instance->CNT = 0;

			dmxCurrentChannel = 0;
			HAL_NVIC_EnableIRQ(DMX512_TIM_IRQ);
			Dmx512DisableUart(dmxHuart);
			HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);

			long time = MARK_BEFORE_BREAK - 1 - dmxHtim->Instance->CNT;
			dmxHtim->Instance->ARR = (time > 0) ? time : 0;
		}
	}
}

void Dmx512Update(TIM_HandleTypeDef *htim2) {
	if (dmxSendState == STATE_MBB) {
		// Mark Before Break finished. Next state is Break. Set TX Low.
		dmxSendState = STATE_BREAK;

		dmxHtim->Instance->CNT = 0;

		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);

		long time = MARK_BREAK - 1 - dmxHtim->Instance->CNT;
		dmxHtim->Instance->ARR = (time > 0) ? time : 0;

	} else if (dmxSendState == STATE_BREAK) {
		// Break finished. Next state is Mark After Break. Set TX high.
		// Account for the time it takes to setup GPIO alternate function mode.
		dmxSendState = STATE_MAB;

		dmxHtim->Instance->CNT = 0;

		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);
		Dmx512EnableUart(dmxHuart);

		long time = MARK_AFTER_BREAK - 1 - dmxHtim->Instance->CNT;
		dmxHtim->Instance->ARR = (time > 0) ? time : 0;

	} else if (dmxSendState == STATE_MAB) {
		// Mark After Break finished. Next send data over USART, then state Mark Before Break.
		dmxSendState = STATE_MBB;
		HAL_NVIC_DisableIRQ(DMX512_TIM_IRQ);

		uint8_t startCode = DMX_START_CODE;
		HAL_UART_IRQHandler(dmxHuart);
		HAL_UART_Transmit_DMA(dmxHuart, &startCode, 1);
	}
}

