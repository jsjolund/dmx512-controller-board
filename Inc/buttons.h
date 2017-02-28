/*
 * buttons.h
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "stm32f4xx_hal.h"
#include "io_exp.h"

#define BUTTONS_I2C_TIMEOUT 100

void ButtonsInit(I2C_HandleTypeDef *hi2cHandle);

#endif /* BUTTONS_H_ */
