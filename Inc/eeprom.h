/*
 * eeprom.h
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#ifndef EEPROM_H_
#define EEPROM_H_

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_conf.h"
#include <stm32f4xx_hal_i2c.h>
#include <stm32f4xx.h>

#define HAL_I2C_MODULE_ENABLED
#define HAL_TIM_MODULE_ENABLED

#define EEPROM_ADDRESS		0xA0
#define EEPROM_PAGE_WRITE_SIZE	128
#define EEPROM_PAGE_READ_SIZE	0xFFFF

void EEPROMInit(I2C_HandleTypeDef* hi2c);
int EEPROMwrite(uint16_t address, uint8_t* MemTarget, uint16_t Size);
int EEPROMread(uint16_t address, uint8_t* MemTarget, uint16_t Size);
int EEPROMfinished(void);
void EEPROM_I2C_EV_IRQHandler(void);
#endif /* EEPROM_H_ */
