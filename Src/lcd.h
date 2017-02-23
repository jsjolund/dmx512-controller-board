/*
 * lcd.h
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#ifndef LCD_H_
#define LCD_H_

void LCDinit(TIM_HandleTypeDef *microSecondHtimHandle, TIM_HandleTypeDef *pwmHtimHandle);
void LCDwrite(char *string);
void LCDcursorPos(uint8_t row, uint8_t column);
void LCDbrightness(uint8_t percent);
void LCDclear(void);
#endif /* LCD_H_ */
