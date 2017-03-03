/*
 * buttons.c
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#include "buttons.h"

static I2C_HandleTypeDef *btnHi2c;

// TODO: These should probably be mapped to whatever buttons we have
// NOTE: They do not have unique identifiers since they map to pin numbers
#define BTN_PIN_F1 GPB1
#define BTN_PIN_F2 GPB3
#define BTN_PIN_F3 GPB5
#define BTN_PIN_F4 GPB7
#define BTN_PIN_F5 GPA1
#define BTN_PIN_F6 GPA3
#define BTN_PIN_F7 GPA5
#define BTN_PIN_F8 GPA7

#define BTN_PIN_S1 GPB1
#define BTN_PIN_S2 GPB3
#define BTN_PIN_S3 GPB5
#define BTN_PIN_S4 GPB7

void ButtonSetLED(int btnPin, uint8_t state) {
	uint16_t gpioState;
	while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	if (state)
		gpioState |= (btnPin >> 1);
	else
		gpioState &= ~(btnPin >> 1);
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}

uint8_t ButtonGetLED(int btnPin) {
	uint16_t status;
	while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &status, sizeof(status), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	return (status & (btnPin >> 1)) ? 1 : 0;
}

void ButtonEvent(int btnPin, uint8_t isPressed) {
	if (isPressed) {
		if (ButtonGetLED(btnPin))
			ButtonSetLED(btnPin, 0);
		else
			ButtonSetLED(btnPin, 1);
	}
	const char * state = isPressed ? "pressed" : "released";
	char * buttonName;
	switch (btnPin) {
	case BTN_PIN_F1:
		buttonName = "BTN_F1";
		break;
	case BTN_PIN_F2:
		buttonName = "BTN_F2";
		break;
	case BTN_PIN_F3:
		buttonName = "BTN_F3";
		break;
	case BTN_PIN_F4:
		buttonName = "BTN_F4";
		break;
	case BTN_PIN_F5:
		buttonName = "BTN_F5";
		break;
	case BTN_PIN_F6:
		buttonName = "BTN_F6";
		break;
	case BTN_PIN_F7:
		buttonName = "BTN_F7";
		break;
	case BTN_PIN_F8:
		buttonName = "BTN_F8";
		break;
	default:
		buttonName = "UNKNOWN";
		break;
	}
	printf("%s %s\r\n", buttonName, state);
}

uint8_t ButtonChangedState(int btnPin, uint16_t flags, uint16_t status, uint8_t *isPressed) {
	if (flags & btnPin) {
		*isPressed = (status & btnPin) ? 0 : 1;
		return 1;
	}
	return 0;
}

void EXTI9_5_IRQHandler(void) {
	const int BUTTONS[] = { BTN_PIN_F1, BTN_PIN_F2, BTN_PIN_F3, BTN_PIN_F4, BTN_PIN_F5, BTN_PIN_F6, BTN_PIN_F7, BTN_PIN_F8 };
	const int NUM_BUTTONS = 8;

	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET) {
		uint16_t flags;
		while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS, INTFA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &flags, sizeof(flags), BUTTONS_I2C_TIMEOUT) != HAL_OK)
			;
		uint16_t status;
		while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &status, sizeof(status), BUTTONS_I2C_TIMEOUT) != HAL_OK)
			;
		uint8_t isPressed;
		int i;
		for (i = 0; i < NUM_BUTTONS; i++)
			if (ButtonChangedState(BUTTONS[i], flags, status, &isPressed))
				ButtonEvent(BUTTONS[i], isPressed);
	}
	EXTI->PR |= B2_Pin;
}

void ButtonsInit(I2C_HandleTypeDef *hi2cHandle) {
	btnHi2c = hi2cHandle;
	// Setup I/O expanders through I2C.

	// Byte mode with IOCON.BANK = 0, INT pins internally connected, INT active-high
	uint8_t settings = IOCON_MIRROR | IOCON_INTPOL;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOCONA, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOCONB, I2C_MEMADD_SIZE_8BIT, &settings, sizeof(settings), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t direction = 0b1010101010101010; // Each even pin is output, odd inputs (1 = Pin is configured as an input)
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IODIRA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// 1 = Enable GPIO input pin for interrupt-on-change event.
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, GPINTENA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &direction, sizeof(direction), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// Interrupt on change for inputs. 0 = Pin value is compared against the previous pin value.
	uint16_t intcon = 0;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, INTCONA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &intcon, sizeof(intcon), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// 1 = Pull-up enabled
	uint16_t pullups = 0b0101010101010101;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, GPPUA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &pullups, sizeof(pullups), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t levels = 0xffff;
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &levels, sizeof(levels), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	// Read once to reset any interrupt related stuff
	uint16_t result;
	while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &result, sizeof(result), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}
