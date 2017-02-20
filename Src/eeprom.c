/*
 * eeprom.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#include "eeprom.h"

I2C_HandleTypeDef *eepromHi2c;
volatile uint8_t eepromWrite;
volatile uint8_t eepromBusy;
volatile uint16_t eepromAddress;
uint8_t *eepromTarget;
volatile uint16_t eepromTargetSize;
volatile uint16_t eepromCounter;

void EEPROMInit(I2C_HandleTypeDef *hi2c) {
	eepromHi2c = hi2c;
}

void EEPROMreadDMA(void) {
	if (eepromCounter < eepromTargetSize) {
		uint16_t sizeDiff = eepromTargetSize - eepromCounter;
		if (sizeDiff >= EEPROM_MAXPKT) {
			// Larger than page read size
			HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, eepromAddress + eepromCounter, I2C_MEMADD_SIZE_16BIT,
					&eepromTarget[eepromCounter], EEPROM_MAXPKT);
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_MAXPKT;
		} else {
			// Smaller than page read size
			HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, eepromAddress + eepromCounter, I2C_MEMADD_SIZE_16BIT,
					&eepromTarget[eepromCounter], sizeDiff);
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
			HAL_I2C_Mem_Write_DMA(eepromHi2c, EEPROM_ADDRESS, eepromAddress + eepromCounter, I2C_MEMADD_SIZE_16BIT,
					&eepromTarget[eepromCounter], EEPROM_MAXPKT);
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += EEPROM_MAXPKT;
		} else {
			// Smaller than page read size
			HAL_I2C_Mem_Write_DMA(eepromHi2c, EEPROM_ADDRESS, eepromAddress + eepromCounter, I2C_MEMADD_SIZE_16BIT,
					&eepromTarget[eepromCounter], sizeDiff);
			HAL_DMA_IRQHandler(eepromHi2c->hdmatx);
			eepromCounter += sizeDiff;
		}
	}
}

int EEPROMbusy(void) {
	return eepromBusy;
}

int EEPROMread(uint16_t address, uint8_t *target, uint16_t targetSize) {
	asm volatile("" ::: "memory");
	if (eepromBusy)
		return 0;
	eepromWrite = 0;
	eepromBusy = 1;
	eepromAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMreadDMA();
	return 1;
}

int EEPROMwrite(uint16_t address, uint8_t *target, uint16_t targetSize) {
	asm volatile("" ::: "memory");
	if (eepromBusy)
		return 0;
	eepromWrite = 1;
	eepromBusy = 1;
	eepromAddress = address;
	eepromTarget = target;
	eepromTargetSize = targetSize;
	eepromCounter = 0;
	EEPROMwriteDMA();
	return 1;
}

void HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	if (hi2c == eepromHi2c) {
		if (eepromCounter >= eepromTargetSize) {
			eepromBusy = 0;
		} else if (!eepromWrite) {
			EEPROMreadDMA();
		}
	}
}

void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c) {
	HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	if (hi2c == eepromHi2c) {
		if (eepromCounter >= eepromTargetSize) {
			eepromBusy = 0;
		} else if (eepromWrite) {
			EEPROMwriteDMA();
		}
	}
}

