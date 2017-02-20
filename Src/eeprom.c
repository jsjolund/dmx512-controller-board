/*
 * eeprom.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#include "eeprom.h"

I2C_HandleTypeDef *eepromHi2c;

volatile uint8_t eepromBusy;
volatile uint16_t address;
volatile uint8_t *MemTarget;
volatile uint16_t Size;
volatile uint16_t Counter;
volatile uint16_t Diff;

void EEPROMInit(I2C_HandleTypeDef *hi2c) {
	eepromHi2c = hi2c;
}

void EEPROMreadDMA(uint16_t address, uint8_t *MemTarget, uint16_t Size) {
	if (Counter < Size) {
		Diff = Size - Counter;
		if (Diff >= EEPROM_MAXPKT) {
			// Larger than page read size
			HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, address + Counter, I2C_MEMADD_SIZE_16BIT, &MemTarget[Counter], EEPROM_MAXPKT);
			Counter += EEPROM_MAXPKT;
		} else {
			// Smaller than page read size
			HAL_I2C_Mem_Read_DMA(eepromHi2c, EEPROM_ADDRESS, address + Counter, I2C_MEMADD_SIZE_16BIT, &MemTarget[Counter], Diff);
			Counter += Diff;
		}
	}
}

int EEPROMread(uint16_t address, uint8_t *MemTarget, uint16_t Size) {
	if (eepromBusy)
		return 0;
	HAL_NVIC_DisableIRQ(I2C2_EV_IRQn);
	eepromBusy = 1;
	HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);
	EEPROMreadDMA(address, MemTarget, Size);
	return 1;
}

int EEPROMwrite(uint16_t address, uint8_t *MemTarget, uint16_t Size) {
	if (eepromBusy)
		return 0;
	uint16_t Counter = 0;
	HAL_StatusTypeDef Result = HAL_OK;
	while (Counter < Size && Result == HAL_OK) {
		uint16_t Diff = Size - Counter;

		if (Diff >= EEPROM_MAXPKT) {
			Result = HAL_I2C_Mem_Write(eepromHi2c, EEPROM_ADDRESS, address + Counter, I2C_MEMADD_SIZE_16BIT, &MemTarget[Counter], EEPROM_MAXPKT, EEPROM_TIMEOUT);
			Counter += EEPROM_MAXPKT;
		} else {
			Result = HAL_I2C_Mem_Write(eepromHi2c, EEPROM_ADDRESS, address + Counter, I2C_MEMADD_SIZE_16BIT, &MemTarget[Counter], Diff, EEPROM_TIMEOUT);
			Counter += Diff;
		}
		HAL_Delay(EEPROM_WRITE);
	}
	return 1;
}

void HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef *hi2c) {
	if (hi2c == eepromHi2c) {
		if (Counter >= Size) {
			eepromBusy = 0;
		} else {
			EEPROMreadDMA(address, MemTarget, Size);
		}
	}
}
