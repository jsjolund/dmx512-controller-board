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
void LCDbrightness(uint8_t percent);
void LCDclear(void);
void LCD_HAL_I2C_MemRxCpltCallback(I2C_HandleTypeDef *hi2cHandle);
void LCD_HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2cHandle);
#endif /* LCD_H_ */
