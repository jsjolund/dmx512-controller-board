/*
 * serial.h
 *
 *  Created on: Jan 28, 2017
 *      Author: user
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include "stm32f4xx_hal.h"
#include "dmx512.h"
#include "shell.h"
#include "sys_public.h"

#define RX_BUFFER_MAX 20
#define TX_BUFFER_MAX 60

void SerialInit(UART_HandleTypeDef *huart2);
void SerialTransmit(char *ptr, int len);

#endif /* SERIAL_H_ */
