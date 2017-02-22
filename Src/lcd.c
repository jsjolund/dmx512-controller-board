/*
 * lcd.c
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#include "stm32f4xx_hal.h"
#include "lcd.h"

#define LCD_RS_Pin GPIO_PIN_1
#define LCD_RS_GPIO_Port GPIOB
#define LCD_E_Pin GPIO_PIN_2
#define LCD_E_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_2
#define LCD_D4_GPIO_Port GPIOD
#define LCD_D5_Pin GPIO_PIN_10
#define LCD_D5_GPIO_Port GPIOC
#define LCD_D6_Pin GPIO_PIN_11
#define LCD_D6_GPIO_Port GPIOC
#define LCD_D7_Pin GPIO_PIN_12
#define LCD_D7_GPIO_Port GPIOC

#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_CURSOR_HOME 0x02
#define LCD_CMD_CURSOR_RIGHT_NO_SHIFT 0x06
#define LCD_CMD_OFF 0x08
#define LCD_CMD_ON_CURSOR_OFF 0x0C
#define LCD_CMD_ON_CURSOR_STEADY 0x0E
#define LCD_CMD_ON_CURSOR_BLINK 0x0F

void __attribute__((optimize("O0"))) Delay(uint64_t delay) {
	while (delay--)
		;
}

void LCDsendHalfByte(uint8_t rs, uint8_t halfByte) {
	LCD_E_GPIO_Port->ODR &= ~(LCD_E_Pin);
	LCD_RS_GPIO_Port->ODR = (rs) ? (LCD_RS_GPIO_Port->ODR | (LCD_RS_Pin)) : (LCD_RS_GPIO_Port->ODR & ~(LCD_RS_Pin));
	LCD_D4_GPIO_Port->ODR = (1 & (halfByte >> 0)) ? (LCD_D4_GPIO_Port->ODR | (LCD_D4_Pin)) : (LCD_D4_GPIO_Port->ODR & ~(LCD_D4_Pin)); // D4
	LCD_D5_GPIO_Port->ODR = (1 & (halfByte >> 1)) ? (LCD_D5_GPIO_Port->ODR | (LCD_D5_Pin)) : (LCD_D5_GPIO_Port->ODR & ~(LCD_D5_Pin)); // D5
	LCD_D6_GPIO_Port->ODR = (1 & (halfByte >> 2)) ? (LCD_D6_GPIO_Port->ODR | (LCD_D6_Pin)) : (LCD_D6_GPIO_Port->ODR & ~(LCD_D6_Pin)); // D6
	LCD_D7_GPIO_Port->ODR = (1 & (halfByte >> 3)) ? (LCD_D7_GPIO_Port->ODR | (LCD_D7_Pin)) : (LCD_D7_GPIO_Port->ODR & ~(LCD_D7_Pin)); // D7
	Delay(0x6F);
	LCD_E_GPIO_Port->ODR |= (LCD_E_Pin);
	Delay(0x6F);
	LCD_E_GPIO_Port->ODR &= ~(LCD_E_Pin);
	Delay(0x6F);
}

void LCDsendCmd(uint8_t cmd) {
	LCDsendHalfByte(0, cmd >> 4);
	LCDsendHalfByte(0, cmd & 0xF);
}

void LCDsendChar(char c) {
	LCDsendHalfByte(1, c >> 4);
	LCDsendHalfByte(1, c & 0xF);
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

void LCDwrite(char *string) {
	while (*string)
		LCDsendChar(*string++);
}

void LCDinit(void) {
	__HAL_RCC_GPIOB_CLK_ENABLE()
	;
	__HAL_RCC_GPIOC_CLK_ENABLE()
	;
	__HAL_RCC_GPIOD_CLK_ENABLE()
	;

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = LCD_RS_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_RS_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_E_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_E_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D4_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_D4_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D5_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_D5_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D6_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_D6_GPIO_Port, &GPIO_InitStruct);

	GPIO_InitStruct.Pin = LCD_D7_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_D7_GPIO_Port, &GPIO_InitStruct);

	HAL_GPIO_WritePin(LCD_RS_GPIO_Port, LCD_RS_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_E_GPIO_Port, LCD_E_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D4_GPIO_Port, LCD_D4_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D5_GPIO_Port, LCD_D5_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D6_GPIO_Port, LCD_D6_Pin, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(LCD_D7_GPIO_Port, LCD_D7_Pin, GPIO_PIN_RESET);

	// http://web.alfredstate.edu/weimandn/lcd/lcd_initialization/lcd_initialization_index.html
	HAL_Delay(110);
	LCDsendHalfByte(0, 0b0011); // Special case of 'Function Set' (lower four bits are irrelevant)
	HAL_Delay(5);
	LCDsendHalfByte(0, 0b0011); // Special case of 'Function Set' (lower four bits are irrelevant)
	HAL_Delay(1); // >100us
	LCDsendHalfByte(0, 0b0011); // Special case of 'Function Set' (lower four bits are irrelevant)
	HAL_Delay(1); // >100us
	LCDsendHalfByte(0, 0b0010); // Initial 'Function Set' to change interface (lower four bits are irrelevant)
	HAL_Delay(1); // >100us
	LCDsendHalfByte(0, 0b0010); // 'Function Set'
	LCDsendHalfByte(0, 0b1000); // NF** (N=0; 1-line display, F=0 5x8 dot font)
	HAL_Delay(1); // > 53 us
	LCDsendCmd(LCD_CMD_ON_CURSOR_OFF);
	HAL_Delay(1); // > 53 us
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(4); // > 3 ms
	LCDsendCmd(LCD_CMD_CURSOR_RIGHT_NO_SHIFT);
	HAL_Delay(60); // > 53 us
	LCDsendCmd(LCD_CMD_CURSOR_HOME);

}

