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
#define STATE_DMA 8

#define MARK_BEFORE_BREAK 100
#define MARK_BREAK 100
#define MARK_AFTER_BREAK 100

#define NUM_CHANNELS 3

uint16_t currentChannel = 0;
uint8_t data[NUM_CHANNELS];
int state;

TIM_HandleTypeDef *htim;
UART_HandleTypeDef *huart;

GPIO_InitTypeDef GPIO_InitStruct;

void Dmx512DisableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_DisableIRQ(USART1_IRQn);
	while (HAL_UART_DMAStop(huart1) != HAL_OK)
		;
	GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Dmx512EnableUart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

void Dmx512Init(TIM_HandleTypeDef *htim2, UART_HandleTypeDef *huart1) {
	htim = htim2;
	huart = huart1;

	int i;
	for (i = 0; i < NUM_CHANNELS; i++) {
		data[i] = 0b01010101;
//		data[i] = 0;
	}

	state = STATE_BREAK;
	htim->Init.Period = MARK_BREAK;

	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 1);

	Dmx512DisableUart(huart);

	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);

	HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	HAL_TIM_Base_Init(htim);
	HAL_TIM_Base_Start_IT(htim);

}

//// https://github.com/Lora-net/LoRaMac-node/issues/96
void USART1_IRQHandler(void) {
	HAL_UART_IRQHandler(huart);
	if (USART_SR_TXE & USART1->SR) {
		if (currentChannel < NUM_CHANNELS) {
			uint8_t d = data[currentChannel];

			HAL_UART_Transmit_DMA(huart, &d, 1);
			currentChannel++;
		} else {
			currentChannel = 0;
			Dmx512DisableUart(huart);

			htim->Instance->ARR = MARK_BREAK - 1;
			HAL_NVIC_EnableIRQ(TIM2_IRQn);
			HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);
			while (HAL_TIM_Base_Start_IT(htim) != HAL_OK)
				;
		}
	}
}
void Dmx512Update(TIM_HandleTypeDef *htim2) {
	if (state == STATE_BREAK) {
		state = STATE_MAB;

		while (HAL_TIM_Base_Stop_IT(htim) != HAL_OK)
			;
		htim->Instance->CNT = 0;
		htim->Instance->ARR = MARK_AFTER_BREAK - 1;
		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);
		while (HAL_TIM_Base_Start_IT(htim) != HAL_OK)
			;

	} else if (state == STATE_MAB) {
		state = STATE_BREAK;

		HAL_NVIC_DisableIRQ(TIM2_IRQn);
		while (HAL_TIM_Base_Stop_IT(htim) != HAL_OK)
			;
		Dmx512EnableUart(huart);
		uint8_t d = 0;
		HAL_UART_IRQHandler(huart);
		HAL_UART_Transmit_DMA(huart, &d, 1);
	}
}

