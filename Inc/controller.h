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

int ControllerQueueCmd(enum buttonEnum uCmd, int state);
void ControllerUpdate(void);

#endif
