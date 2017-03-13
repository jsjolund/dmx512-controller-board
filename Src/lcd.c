/*
 * lcd.c
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#include "lcd.h"

static TIM_HandleTypeDef *microSecondHtim;
static TIM_HandleTypeDef *pwmHtim;
static I2C_HandleTypeDef *lcdHi2c;

static volatile uint8_t targetBrightness;

void LCD_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle) {
	// Timer interrupt for changing the backlight brightness in steps
	uint8_t currentBrightness = pwmHtim->Instance->CCR1;
	if (currentBrightness < targetBrightness) {
		pwmHtim->Instance->CCR1 = currentBrightness + 1;
	} else if (currentBrightness > targetBrightness) {
		pwmHtim->Instance->CCR1 = currentBrightness - 1;
	} else {
		HAL_NVIC_DisableIRQ(LCD_TIM_IRQ);
	}
}

void LCDsetBrightness(uint8_t percent) {
	// Set the brightness of backlight in percent
	targetBrightness = percent;
	pwmHtim->Instance->CCR1 = percent;
}

uint8_t LCDgetBrightness(void) {
	// Get the current backlight in percent
	return pwmHtim->Instance->CCR1;
}

void LCDfadeBrightness(uint8_t percent, uint8_t secondsFade) {
	// Fade the brightness to some percent during some seconds
	targetBrightness = percent;
	if (secondsFade == 0) {
		LCDsetBrightness(targetBrightness);
	} else {
		uint8_t currentBrightness = pwmHtim->Instance->CCR1;
		uint8_t steps = (targetBrightness > currentBrightness) ? targetBrightness - currentBrightness : currentBrightness - targetBrightness;
		microSecondHtim->Instance->ARR = 1000000 / (steps) * secondsFade;
		HAL_NVIC_EnableIRQ(LCD_TIM_IRQ);
		HAL_NVIC_SetPriority(LCD_TIM_IRQ, 5, 0);
	}
}

void LCDsendBytes(uint8_t rs, uint8_t bytes) {
	// Sends a byte (blocking transfer) through I2C
	uint16_t data = (rs) ? LCD_RS_Pin : 0;
	data |= bytes << 8;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	data |= LCD_E_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	data &= ~LCD_E_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &data, sizeof(data), LCD_I2C_TIMEOUT) != HAL_OK)
		;
}

void LCDsendCmd(uint8_t cmd) {
	LCDsendBytes(0, cmd);
}

void LCDsendChar(char character) {
	LCDsendBytes(1, character);
}

void LCDcursorPos(uint8_t row, uint8_t column) {
	// Set the position of the internal LCD cursor
	switch (row) {
	case 0:
		LCDsendCmd(0x80 + column);
		break;
	case 1:
		LCDsendCmd(0xC0 + column);
		break;
	case 2:
		LCDsendCmd(0x90 + column + 4);
		break;
	case 3:
		LCDsendCmd(0xD0 + column + 4);
		break;
	default:
		break;
	}
}

void LCDwrite(char *string) {
	// Write a string to the LCD
	while (*string)
		LCDsendChar(*string++);
}

void LCDclear(void) {
	// Clear the LCD. Should not be used each frame, since this will cause flickering.
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(4); // >3 ms
}

void LCDclearRow(uint8_t row) {
	// Clear a single row. Should not be used each frame, since this will cause flickering.
	LCDcursorPos(row, 0);
	int i;
	for (i = 0; i < 20; i++)
		LCDsendChar(' ');
}

void EXTI0_IRQHandler(void) {
	EXTI->PR |= B0_Pin;
}

void LCDinit(TIM_HandleTypeDef *microSecondHtimHandle, TIM_HandleTypeDef *pwmHtimHandle, I2C_HandleTypeDef *hi2cHandle) {

	lcdHi2c = hi2cHandle;

	// Setup the I/O expander
	// Byte mode with IOCON.BANK = 0, INT pins internally connected, INT active-high
	uint8_t settings = IOCON_MIRROR | IOCON_INTPOL;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOCONA, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOCONB, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t direction = LCD_BUTTON_Pin | LCD_ENCODER_W_Pin | LCD_ENCODER_B_Pin | LCD_ENCODER_A_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IODIRA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// 1 = Enable GPIO input pin for interrupt-on-change event.
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, GPINTENA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// Interrupt on change for inputs. 0 = Pin value is compared against the previous pin value.
	uint16_t intcon = 0;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, INTCONA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &intcon, sizeof(intcon), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// 1 = Pull-up enabled
	uint16_t pullups = ~direction;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, GPPUA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &pullups, sizeof(pullups), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t levels = direction | LCD_BUTTON_LED_Pin;
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &levels, sizeof(levels), LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// Read once to reset any interrupt related stuff
	volatile uint16_t result;
	while (HAL_I2C_Mem_Read(lcdHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &result, sizeof(result), LCD_I2C_TIMEOUT) != HAL_OK)
		;

	// Setup PWM to control backlight brightness
	pwmHtim = pwmHtimHandle;
	HAL_TIM_Base_Start(pwmHtim);
	HAL_TIM_PWM_Start(pwmHtim, LCD_PWM_CHANNEL);
	pwmHtim->Instance->ARR = 100; // Period
	pwmHtim->Instance->CCR1 = 0; // Pulse

	microSecondHtim = microSecondHtimHandle;
	HAL_TIM_Base_Init(microSecondHtim);
	HAL_TIM_Base_Start_IT(microSecondHtim);

	// Wait for LCD to power on fully, then set the correct mode
	HAL_Delay(200);
	LCDsendCmd(LCD_CMD_INIT);
	HAL_Delay(1);
	LCDsendCmd(LCD_CMD_ON_CURSOR_OFF);
	HAL_Delay(1);
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(5);
}
