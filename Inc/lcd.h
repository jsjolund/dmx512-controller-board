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

#define LCD_RS_Pin GPIO_PIN_2
#define LCD_RW_Pin GPIO_PIN_3
#define LCD_E_Pin GPIO_PIN_4

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
void LCD_HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2cHandle);
void LCD_HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2cHandle);
void LCD_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle);
#endif /* LCD_H_ */
