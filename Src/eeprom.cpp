/*
 * eeprom.cpp
 *
 *  Created on: Jan 25, 2017
 *      Author: user
 */

#include "eeprom.h"

Eeprom::Eeprom() {
	// TODO Auto-generated constructor stub

}

Eeprom::~Eeprom() {
	// TODO Auto-generated destructor stub
}

void Eeprom::begin(I2C_HandleTypeDef* i2cPort) {
	//init the eeprom object
	i2c_port = i2cPort;
}
////This is basically just a nicer wrapper
//void Eeprom::readObject(structObject* settings, int section)
//{
//	//Read a screen settings object from eeprom
//	readEEPROM(section * EEPROM_SECTIONSIZE, (uint8_t*) settings, sizeof(StructObject));
//}
//void Eeprom::saveObject(structObject* settings, int section)
//{
//	writeEEPROM(section * EEPROM_SECTIONSIZE, (uint8_t*) settings, sizeof(StructObject));
//}

HAL_StatusTypeDef Eeprom::readEEPROM(uint16_t address, uint8_t* MemTarget,
		uint16_t Size) {
	uint16_t Counter = 0;
	HAL_StatusTypeDef Result = HAL_OK;
	while (Counter < Size && Result == HAL_OK) {
		uint16_t Diff = Size - Counter;

		if (Diff >= EEPROM_MAXPKT) {
			//Multi-Byte
			Result = HAL_I2C_Mem_Read(i2c_port, EEPROM_ADDRESS,
					address + Counter, I2C_MEMADD_SIZE_16BIT,
					&MemTarget[Counter], EEPROM_MAXPKT, EEPROM_TIMEOUT);
			Counter += EEPROM_MAXPKT;
		} else {
			//and the remaining ones...low packet size
			Result = HAL_I2C_Mem_Read(i2c_port, EEPROM_ADDRESS,
					address + Counter, I2C_MEMADD_SIZE_16BIT,
					&MemTarget[Counter], Diff, EEPROM_TIMEOUT);
			Counter += Diff;
		}
		HAL_Delay(EEPROM_WRITE / 2);
	}
	return Result;
}

HAL_StatusTypeDef Eeprom::writeEEPROM(uint16_t address, uint8_t* MemTarget,
		uint16_t Size) {
	uint16_t Counter = 0;
	HAL_StatusTypeDef Result = HAL_OK;
	while (Counter < Size && Result == HAL_OK) {
		uint16_t Diff = Size - Counter;

		if (Diff >= EEPROM_MAXPKT) {
			//Multi-Byte
			Result = HAL_I2C_Mem_Write(i2c_port, EEPROM_ADDRESS,
					address + Counter, I2C_MEMADD_SIZE_16BIT,
					&MemTarget[Counter], EEPROM_MAXPKT, EEPROM_TIMEOUT);
			Counter += EEPROM_MAXPKT;
		} else {
			//and the remaining ones...low packet size
			Result = HAL_I2C_Mem_Write(i2c_port, EEPROM_ADDRESS,
					address + Counter, I2C_MEMADD_SIZE_16BIT,
					&MemTarget[Counter], Diff, EEPROM_TIMEOUT);
			Counter += Diff;
		}
		HAL_Delay(EEPROM_WRITE);
	}
	return Result;
}
