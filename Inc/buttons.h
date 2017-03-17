/*
 * buttons.h
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#ifndef BUTTONS_H_
#define BUTTONS_H_

#include "stm32f4xx_hal.h"

typedef const enum buttonEnum {
	BTN_F1,
	BTN_F2,
	BTN_F3,
	BTN_F4,
	BTN_F5,
	BTN_F6,
	BTN_F7,
	BTN_F8,
	BTN_S1,
	BTN_S2,
	BTN_S3,
	BTN_S4,
	BTN_LCD,
	BTN_ENC,
	ENC_CW,
	ENC_CCW,
	BTN_NONE
} buttonEnum;

typedef struct buttonStruct {
	const enum buttonEnum btn;
	const char *btnName;
	const int btnPin;
	const int ledPin;
	const int ioexp_address;
	I2C_HandleTypeDef *hi2c;
} buttonStruct;

#define BUTTON_RELEASED 0
#define BUTTON_PRESSED 1

#include "io_exp.h"
#include "controller.h"

#define BUTTONS_I2C_TIMEOUT 100

#define IOEXP_ADDRESS_F (IOEXP_ADDRESS + 0)
#define IOEXP_ADDRESS_S (IOEXP_ADDRESS + 2)

extern struct buttonStruct buttonMap[];
extern volatile uint8_t lcdButtonLEDstate;

void ButtonsInit(I2C_HandleTypeDef *hi2cHandle);
void ButtonEvent(struct buttonStruct *button, int isPressed);
uint8_t ButtonGetLED(struct buttonStruct *button);
void ButtonSetLED(struct buttonStruct *button, uint8_t state);
void ButtonSetFNbuttonLEDS(uint8_t state);

#endif /* BUTTONS_H_ */
