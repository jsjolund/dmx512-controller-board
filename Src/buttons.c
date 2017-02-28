/*
 * buttons.c
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#include "buttons.h"

static I2C_HandleTypeDef *btnHi2c;

void EXTI9_5_IRQHandler(void) {
	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_RESET) {
		HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
	}
	EXTI->PR |= B2_Pin;
}

void ButtonsInit(I2C_HandleTypeDef *hi2cHandle) {

	btnHi2c = hi2cHandle;

	// Byte mode with IOCON.BANK = 0, INT pins internally connected, INT active-high
	uint8_t settings = IOCON_MIRROR | IOCON_INTPOL;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOCONA, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOCONB, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;

	uint16_t direction = 0b1010101010101010; // Each even pin is output, odd inputs (1 = Pin is configured as an input)
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IODIRA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// 1 = Enable GPIO input pin for interrupt-on-change event.
	direction = 0xFFFF;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, GPINTENA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// These bits set the compare value for pins configured for interrupt-on-change from defaults <7:0>. Refer to INTCON.
	// If the associated pin level is the opposite from the register bit, an interrupt occurs.
	direction = 0xFFFF;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, DEFVALA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	direction = 0;
	// 0 = Pin value is compared against the previous pin value.
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, INTCONA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t pullups = 0b0101010101010101; // 1 = Pull-up enabled
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, GPPUA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &pullups, sizeof(pullups), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t levels = 0b0101010101010101;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &levels, sizeof(levels), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;

}
