/*
 * dmx512.h
 *
 *  Created on: Feb 16, 2017
 *      Author: johsjl-1
 */

#ifndef DMX512_H_
#define DMX512_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx.h"
#include "stm32f4xx_it.h"

#define STATE_MBB 1
#define STATE_BREAK 2
#define STATE_MAB 4
#define STATE_DMA 8

#define MARK_BEFORE_BREAK 24
#define MARK_BREAK 176
#define MARK_AFTER_BREAK 24

#define DMX_CHANNELS 512

#define DMX_START_CODE 0

int Dmx512SetChannelValue(uint16_t channel, uint8_t value);
void Dmx512Init(TIM_HandleTypeDef *htimHandle, UART_HandleTypeDef *huartHandle);
void Dmx512_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle);

#endif /* DMX512_H_ */
