/*
 * dmx512.c
 *
 *  Created on: Feb 16, 2017
 *      Author: johsjl-1
 */
#include "dmx512.h"

static uint8_t dmxData[DMX_CHANNELS + 1];
static volatile uint8_t dmxSendState;

static TIM_HandleTypeDef *dmxHtim;
static UART_HandleTypeDef *dmxHuart;

static GPIO_InitTypeDef dmx_GPIO_InitStruct;

int Dmx512SetChannelValue(uint16_t channel, uint8_t value) {
	if (channel >= DMX_CHANNELS)
		return 0;
	dmxData[channel + 1] = value;
	return 1;
}

void Dmx512DisableUsart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_DisableIRQ(DMX_USART_IRQ);

	dmx_GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	dmx_GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	dmx_GPIO_InitStruct.Pull = GPIO_NOPULL;
	dmx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	HAL_GPIO_Init(DMX_TX_GPIO_Port, &dmx_GPIO_InitStruct);
}

void Dmx512EnableUsart(UART_HandleTypeDef *huart1) {
	HAL_NVIC_EnableIRQ(DMX_USART_IRQ);

	dmx_GPIO_InitStruct.Pin = DMX_TX_Pin | DMX_RX_Pin;
	dmx_GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	dmx_GPIO_InitStruct.Pull = GPIO_PULLUP;
	dmx_GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	dmx_GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(DMX_TX_GPIO_Port, &dmx_GPIO_InitStruct);
}

void Dmx512Init(TIM_HandleTypeDef *htimHandle, UART_HandleTypeDef *huartHandle) {
	dmxHtim = htimHandle;
	dmxHuart = huartHandle;

	int i;
	for (i = 0; i < DMX_CHANNELS; i++)
		Dmx512SetChannelValue(i, 0);

	dmxSendState = STATE_BREAK;
	dmxHtim->Init.Period = MARK_BREAK;

	HAL_NVIC_EnableIRQ(DMX_USART_IRQ);
	HAL_NVIC_SetPriority(DMX_USART_IRQ, 0, 0);

	Dmx512DisableUsart(dmxHuart);

	HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);

	HAL_NVIC_SetPriority(DMX_TIM_IRQ, 0, 0);
	HAL_NVIC_EnableIRQ(DMX_TIM_IRQ);

	HAL_TIM_Base_Init(dmxHtim);
	HAL_TIM_Base_Start_IT(dmxHtim);
}

void USART1_IRQHandler(void) {
	// This function is called on USART interrupt
	HAL_UART_IRQHandler(dmxHuart);

	if (USART_SR_TC & DMX_USART->SR) {
		// Finished sending data over USART. Next is Mark Before Break. Set TX high.
		// Account for the time it takes to setup GPIO output mode.
		dmxSendState = STATE_MBB;
		HAL_TIM_Base_Start(dmxHtim);
		dmxHtim->Instance->CNT = 0;
		Dmx512DisableUsart(dmxHuart);
		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);
		HAL_TIM_Base_Stop(dmxHtim);
		uint32_t elapsedTime = dmxHtim->Instance->CNT;
		long time = MARK_BEFORE_BREAK - 1 - elapsedTime;
		dmxHtim->Instance->ARR = (time > 0) ? time : 1;
		HAL_NVIC_EnableIRQ(DMX_TIM_IRQ);
		HAL_TIM_Base_Start_IT(dmxHtim);
	}
}

void Dmx512_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle) {
	// This function is called on timer interrupt.
	if (dmxSendState == STATE_MBB) {
		// Mark Before Break finished. Next state is Break. Set TX Low.
		dmxSendState = STATE_BREAK;
		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_RESET);
		dmxHtim->Instance->ARR = MARK_BREAK;

	} else if (dmxSendState == STATE_BREAK) {
		// Break finished. Next state is Mark After Break. Set TX high.
		// Account for the time it takes to setup GPIO alternate function mode.
		dmxSendState = STATE_MAB;
		HAL_TIM_Base_Stop_IT(dmxHtim);
		HAL_TIM_Base_Start(dmxHtim);
		dmxHtim->Instance->CNT = 0;
		HAL_GPIO_WritePin(DMX_TX_GPIO_Port, DMX_TX_Pin, GPIO_PIN_SET);
		Dmx512EnableUsart(dmxHuart);
		uint32_t elapsedTime = dmxHtim->Instance->CNT;
		long time = MARK_AFTER_BREAK - 1 - elapsedTime;
		dmxHtim->Instance->ARR = (time > 0) ? time : 1;
		HAL_TIM_Base_Start_IT(dmxHtim);

	} else if (dmxSendState == STATE_MAB) {
		// Mark After Break finished. Next send data over USART, then state Mark Before Break.
		dmxSendState = STATE_DMA;
		HAL_TIM_Base_Stop_IT(dmxHtim);
		HAL_NVIC_DisableIRQ(DMX_TIM_IRQ);
		while (HAL_UART_Transmit_DMA(dmxHuart, dmxData, DMX_CHANNELS + 1) != HAL_OK)
			;
		HAL_UART_IRQHandler(dmxHuart);
	}
}

