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

#define MARK_BEFORE_BREAK 12
#define MARK_BREAK 100
#define MARK_AFTER_BREAK 12

#define DMX_CHANNELS 512

#define DMX_START_CODE 0

void Dmx512Init(TIM_HandleTypeDef *htim2, UART_HandleTypeDef *huart1);
void Dmx512Update(TIM_HandleTypeDef *htim2);

#endif /* DMX512_H_ */
