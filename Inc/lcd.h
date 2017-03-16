/*
 * lcd.h
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#ifndef LCD_H_
#define LCD_H_

#include "stm32f4xx_hal.h"
#include "io_exp.h"

#define LCD_ROT_ENC_CW 0xFFFF
#define LCD_ROT_ENC_CCW 0xFFFE

#define LCD_ROT_ENC_B_Pin GPA3
#define LCD_ROT_ENC_A_Pin GPA4
#define LCD_RS_Pin GPA5
#define LCD_RW_Pin GPA6
#define LCD_E_Pin GPA7

#define LCD_I2C_TIMEOUT 100

#define LCD_CMD_CLEAR 0x01
#define LCD_CMD_CURSOR_HOME 0x02
#define LCD_CMD_CURSOR_RIGHT_NO_SHIFT 0x06
#define LCD_CMD_OFF 0x08
#define LCD_CMD_ON_CURSOR_OFF 0x0C
#define LCD_CMD_ON_CURSOR_STEADY 0x0E
#define LCD_CMD_ON_CURSOR_BLINK 0x0F
#define LCD_CMD_INIT 0x38

void LCDinit(TIM_HandleTypeDef *microSecondHtimHandle, TIM_HandleTypeDef *pwmHtimHandle, I2C_HandleTypeDef *hi2cHandle);
void LCDwrite(char *string);
void LCDcursorPos(uint8_t row, uint8_t column);
uint8_t LCDgetBrightness(void);
void LCDsetBrightness(uint8_t percent);
void LCDfadeBrightness(uint8_t percent, uint8_t secondsFade);
void LCDclear(void);
void LCDclearRow(uint8_t row);
void LCD_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle);
#endif /* LCD_H_ */
