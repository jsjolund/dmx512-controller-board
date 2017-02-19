/*
 * dmx512.c
 *
 *  Created on: Feb 16, 2017
 *      Author: johsjl-1
 */
#include "dmx512.h"

#define STATE_MBB 1
#define STATE_BREAK 2
#define STATE_MAB 4

#define MARK_BEFORE_BREAK 12
#define MARK_BREAK 100
#define MARK_AFTER_BREAK 12

#define DMX_CHANNELS 512

#define DMX_START_CODE 0

uint16_t dmxCurrentChannel = 0;
uint8_t dmxData[DMX_CHANNELS];
uint8_t dmxSendState;

TIM_HandleTypeDef *dmxHtim;
UART_HandleTypeDef *dmxHuart;

GPIO_InitTypeDef dmx_GPIO_InitStruct;

void Dmx512DisableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_DisableIRQ(USART1_IRQn);

	dmx_GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	dmx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	dmx_GPIO_InitStruct.Pull = GPIO_NOPULL;
	dmx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(GPIOA, &dmx_GPIO_InitStruct);
}

void Dmx512EnableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_EnableIRQ(USART1_IRQn);

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
		dmxData[i] = 0b01010101;
	}

	dmxSendState = STATE_BREAK;
	dmxHtim->Init.Period = MARK_BREAK;

	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);

	Dmx512DisableUart(dmxHuart);

	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_Base_Init(dmxHtim);
	HAL_TIM_Base_Start_IT(dmxHtim);
}



// https://github.com/Lora-net/LoRaMac-node/issues/96
void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(dmxHuart);
	if (USART_SR_TXE & USART1->SR) {
		if (dmxCurrentChannel < DMX_CHANNELS) {
			uint8_t d = dmxData[dmxCurrentChannel];
			HAL_UART_Transmit_DMA(dmxHuart, &d, 1);
			dmxCurrentChannel++;
		} else {
			dmxCurrentChannel = 0;

			dmxHtim->Instance->CNT = 0;
			dmxHtim->Instance->ARR = MARK_BEFORE_BREAK - 1;
			HAL_NVIC_EnableIRQ(TIM2_IRQn);

			Dmx512DisableUart(dmxHuart);
			HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);
		}
	}
}
void Dmx512Update(TIM_HandleTypeDef *htim2) {
	if (dmxSendState == STATE_MBB) {
		dmxSendState = STATE_BREAK;
		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);
		dmxHtim->Instance->CNT = 0;
		dmxHtim->Instance->ARR = MARK_BREAK - 1;

	} else if (dmxSendState == STATE_BREAK) {
		dmxSendState = STATE_MAB;

		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);

		dmxHtim->Instance->CNT = 0;
		dmxHtim->Instance->ARR = MARK_AFTER_BREAK - 1;

		Dmx512EnableUart(dmxHuart);

	} else if (dmxSendState == STATE_MAB) {
		dmxSendState = STATE_MBB;

		uint8_t startCode = DMX_START_CODE;
		HAL_UART_IRQHandler(dmxHuart);
		HAL_UART_Transmit_DMA(dmxHuart, &startCode, 1);

		HAL_NVIC_DisableIRQ(TIM2_IRQn);
	}
}

