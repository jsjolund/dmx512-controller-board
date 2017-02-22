/*
 * lcd.h
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#ifndef LCD_H_
#define LCD_H_

void LCDinit(void);
void LCDwrite(char *string);
void LCDcursorPos(uint8_t row, uint8_t column);

#endif /* LCD_H_ */
