/*
 * i2c.c
 *
 *  Created on: Feb 24, 2017
 *      Author: johsjl-1
 */


#include "i2c.h"

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2cHandle) {
	EEPROM_HAL_I2C_MemRxCpltCallback(hi2cHandle);
	LCD_HAL_I2C_MemRxCpltCallback(hi2cHandle);
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2cHandle) {
	EEPROM_HAL_I2C_MemTxCpltCallback(hi2cHandle);
	LCD_HAL_I2C_MemTxCpltCallback(hi2cHandle);
}
