/*
 * lcd.h
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#ifndef LCD_H_
#define LCD_H_

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
