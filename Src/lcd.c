/*
 * lcd.c
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#include "stm32f4xx_hal.h"
#include "lcd.h"
#include "io_exp.h"

#define LCD_RS_Pin GPIO_PIN_2
#define LCD_RW_Pin GPIO_PIN_3
#define LCD_E_Pin GPIO_PIN_4

#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_CURSOR_HOME 0x02
#define LCD_CMD_CURSOR_RIGHT_NO_SHIFT 0x06
#define LCD_CMD_OFF 0x08
#define LCD_CMD_ON_CURSOR_OFF 0x0C
#define LCD_CMD_ON_CURSOR_STEADY 0x0E
#define LCD_CMD_ON_CURSOR_BLINK 0x0F
#define LCD_CMD_INIT 0x38

static TIM_HandleTypeDef *microSecondHtim;
static TIM_HandleTypeDef *pwmHtim;
static I2C_HandleTypeDef *lcdHi2c;

void LCD_HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2cHandle) {

}

void LCD_HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2cHandle) {

}

void MicroDelay(uint16_t micros) {
	uint32_t start = microSecondHtim->Instance->CNT;
	while (microSecondHtim->Instance->CNT - start < micros)
		;
}

void LCDsendBytes(uint8_t rs, uint8_t bytes) {
	uint16_t data = (rs) ? LCD_RS_Pin : 0;
	data |= bytes << 8;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), 100) != HAL_OK)
		;
	data |= LCD_E_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), 100) != HAL_OK)
		;
	data &= ~LCD_E_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), 100) != HAL_OK)
		;
}

void LCDsendCmd(uint8_t cmd) {
	LCDsendBytes(0, cmd);
}

void LCDsendChar(char character) {
	LCDsendBytes(1, character);
}

void LCDcursorPos(uint8_t row, uint8_t column) {
	switch (row) {
	case 0:
		LCDsendCmd(0x80 + column);
		break;
	case 1:
		LCDsendCmd(0xC0 + column);
		break;
	case 2:
		LCDsendCmd(0x90 + column);
		break;
	case 3:
		LCDsendCmd(0xD0 + column);
		break;
	default:
		break;
	}
}

void LCDbrightness(uint8_t percent) {
	pwmHtim->Instance->CCR1 = percent;
}

void LCDwrite(char *string) {
	while (*string)
		LCDsendChar(*string++);
}

void LCDclear(void) {
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(4); // >3 ms
}

void LCDclearRow(uint8_t row) {
	LCDcursorPos(row, 0);
	int i;
	for (i = 0; i < 16; i++)
		LCDsendChar(' ');
}

void LCDinit(TIM_HandleTypeDef *microSecondHtimHandle, TIM_HandleTypeDef *pwmHtimHandle, I2C_HandleTypeDef *hi2cHandle) {

	lcdHi2c = hi2cHandle;

	uint8_t settings = 0; // Byte mode with IOCON.BANK = 0, no interrupts.
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOCONA, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), 100) != HAL_OK)
		;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOCONB, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), 100) != HAL_OK)
		;
	uint16_t direction = 0; // Outputs only
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IODIRA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), 100) != HAL_OK)
		;
	uint16_t levels = 0;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &levels, sizeof(levels), 100) != HAL_OK)
		;

	microSecondHtim = microSecondHtimHandle;
	HAL_TIM_Base_Start(microSecondHtim);

	pwmHtim = pwmHtimHandle;
	HAL_TIM_Base_Start(pwmHtim);
	HAL_TIM_PWM_Start(pwmHtim, LCD_PWM_CHANNEL);
	pwmHtim->Instance->ARR = 100; // Period
	pwmHtim->Instance->CCR1 = 100; // Pulse

	HAL_Delay(200); // Wait for LCD to power on fully
	LCDsendCmd(LCD_CMD_INIT);
	MicroDelay(200);
	LCDsendCmd(LCD_CMD_ON_CURSOR_OFF);
	MicroDelay(100);
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(5);
}
