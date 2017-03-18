/*========================================================

 Description:

 Author: Sam Karimi
 ========================================================*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

#include "sys_public.h"
#include "buttons.h"
#include "lcd.h"
#include "dmx512.h"
#include "eeprom.h"
#include "serial.h"

#define MENU_ITEM_SAVE_PROGRAM 0
#define MENU_ITEM_LCD_BACKLIGHT 1
#define MENU_ITEM_USART_PASSHTHROUGH 2
#define MENU_ITEM_ABOUT_SUNSHINE 3

#define MENU_CURSOR_MAX 3

int ControllerQueueCmd(enum buttonEnum uCmd, int buttonState);
void ControllerEditProgram(enum buttonEnum button, int buttonState);
void ControllerUpdate(void);

#endif
