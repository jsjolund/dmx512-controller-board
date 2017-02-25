/*
 * eeprom.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#include "eeprom.h"

static I2C_HandleTypeDef *eepromHi2c;
static volatile uint8_t eepromWrite;
static volatile uint8_t eepromBusy;
static uint8_t *eepromTarget = 0;
static volatile uint16_t eepromTargetAddress = 0;
static volatile uint16_t eepromTargetSize = 0;
static volatile uint16_t eepromCounter = 0;

void EEPROMInit(I2C_HandleTypeDef *hi2c) {
	eepromHi2c = hi2c;
}

void EEPROMreadDMA(void) {
	if (eepromCounter < eepromTargetSize) {
		uint16_t sizeDiff = eepromTargetSize - eepromCounter;
		if (sizeDiff >= EEPROM_MAXPKT) {
			// Larger than page read size
			while (HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], EEPROM_MAXPKT) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_MAXPKT;
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
		if (sizeDiff >= EEPROM_MAXPKT) {
			// Larger than page read size
			while (HAL_I2C_Mem_Write_DMA(eepromHi2c, EEPROM_ADDRESS, eepromTargetAddress + eepromCounter,
			I2C_MEMADD_SIZE_16BIT, &eepromTarget[eepromCounter], EEPROM_MAXPKT) != HAL_OK)
				;
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_MAXPKT;
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

int EEPROMbusy(void) {
	return eepromBusy;
}

int EEPROMread(uint16_t address, uint8_t *target, uint16_t targetSize) {
	if (eepromBusy)
		return 0;
	eepromWrite = 0;
	eepromBusy = 1;
	eepromTargetAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMreadDMA();
	return 1;
}

int EEPROMwrite(uint16_t address, uint8_t *target, uint16_t targetSize) {
	if (eepromBusy)
		return 0;
	eepromWrite = 1;
	eepromBusy = 1;
	eepromTargetAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMwriteDMA();
	return 1;
}

void EEPROM_HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == eepromHi2c) {
		if (eepromCounter < eepromTargetSize && !eepromWrite) {
			EEPROMreadDMA();
		} else if (!eepromWrite) {
			eepromBusy = 0;
		}
	}
}

void EEPROM_HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == eepromHi2c) {
		if (eepromCounter < eepromTargetSize && eepromWrite) {
			EEPROMwriteDMA();
		} else if (eepromWrite) {
			eepromBusy = 0;
		}
	}
}
