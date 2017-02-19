/*
 * serial.h
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stm32f4xx_hal.h"


void SerialInit(UART_HandleTypeDef *huart2);
void SerialTransmit(char *ptr, int len);

#endif /* SERIAL_H_ */
