/*
 * eeprom.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#include "eeprom.h"

#define EEPROM_WRITE_STATE 0
#define EEPROM_READ_STATE 1

static I2C_HandleTypeDef *eepromHi2c;
static volatile int eepromState;
static uint8_t *eepromTarget = 0;
static volatile uint16_t eepromTargetAddress = 0;
static volatile uint16_t eepromTargetSize = 0;
static volatile uint32_t eepromCounter = 0;

void EEPROMInit(I2C_HandleTypeDef *hi2c) {
	eepromHi2c = hi2c;
}

void EEPROMreadDMA(void) {
	if (eepromCounter < eepromTargetSize) {
		uint16_t sizeDiff = eepromTargetSize - eepromCounter;
		if (sizeDiff >= EEPROM_PAGE_READ_SIZE) {
			// Larger than page read size
			while (HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], EEPROM_PAGE_READ_SIZE) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_PAGE_READ_SIZE;
		} else {
			// Smaller than page read size
			while (HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], sizeDiff) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += sizeDiff;
		}
	}
}

void EEPROMwriteDMA(void) {
	if (eepromCounter < eepromTargetSize) {
		uint16_t sizeDiff = eepromTargetSize - eepromCounter;
		if (sizeDiff >= EEPROM_PAGE_WRITE_SIZE) {
			// Larger than page read size
			while (HAL_I2C_Mem_Write_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], EEPROM_PAGE_WRITE_SIZE) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_PAGE_WRITE_SIZE;
		} else {
			// Smaller than page read size
			while (HAL_I2C_Mem_Write_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], sizeDiff) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += sizeDiff;
		}
	}
}

int EEPROMfinished(void) {
	HAL_I2C_StateTypeDef hi2cState = HAL_I2C_GetState(eepromHi2c);
	if ((hi2cState == HAL_I2C_STATE_READY) && (eepromCounter >= eepromTargetSize))
		return 1;
	return 0;
}

int EEPROMread(uint16_t address, uint8_t *target, uint16_t targetSize) {
	if (!EEPROMfinished())
		return 0;
	eepromState = EEPROM_READ_STATE;
	eepromTargetAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMreadDMA();
	return 1;
}

int EEPROMwrite(uint16_t address, uint8_t *target, uint16_t targetSize) {
	if (!EEPROMfinished())
		return 0;
	eepromState = EEPROM_WRITE_STATE;
	eepromTargetAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMwriteDMA();
	return 1;
}

void EEPROM_I2C_EV_IRQHandler(void) {
	HAL_I2C_StateTypeDef hi2cState = HAL_I2C_GetState(eepromHi2c);
	if (hi2cState == HAL_I2C_STATE_READY) {
		if (eepromCounter < eepromTargetSize) {
			if (eepromState == EEPROM_WRITE_STATE) {
				EEPROMwriteDMA();
			} else {
				EEPROMreadDMA();
			}
		}
	}
}

