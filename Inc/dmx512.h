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

void Dmx512Init(TIM_HandleTypeDef *htim2, UART_HandleTypeDef *huart1);
void Dmx512Update(TIM_HandleTypeDef *htim2);

#endif /* DMX512_H_ */
