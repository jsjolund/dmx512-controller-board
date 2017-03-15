/*
 * buttons.c
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#include "buttons.h"

static I2C_HandleTypeDef *btnHi2c;

// NOTE: These do not have unique identifiers since they map to pin numbers
#define BTN_PIN_F1 GPA1
#define BTN_PIN_F2 GPA3
#define BTN_PIN_F3 GPA5
#define BTN_PIN_F4 GPA7
#define BTN_PIN_F5 GPB7
#define BTN_PIN_F6 GPB5
#define BTN_PIN_F7 GPB3
#define BTN_PIN_F8 GPB1

#define BTN_PIN_S1 GPB1
#define BTN_PIN_S2 GPB3
#define BTN_PIN_S3 GPB5
#define BTN_PIN_S4 GPB7

#define IOEXP_ADDRESS_F (IOEXP_ADDRESS + 0)
#define IOEXP_ADDRESS_S (IOEXP_ADDRESS + 2)

void Button_I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Read(btnHi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}

void Button_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Write(btnHi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}

void ButtonSetLED(int devAddress, int btnPin, uint8_t state) {
	uint16_t gpioState;
	Button_I2C_Read(devAddress, IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
	if (state)
		gpioState |= (btnPin >> 1);
	else
		gpioState &= ~(btnPin >> 1);
	Button_I2C_Write(devAddress, IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
}

uint8_t ButtonGetLED(int devAddress, int btnPin) {
	uint16_t gpioState;
	Button_I2C_Read(devAddress, IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
	return (gpioState & (btnPin >> 1)) ? 1 : 0;
}

void ButtonEvent(int devAddress, int btnPin, uint8_t isPressed) {
	if (isPressed) {
		if (ButtonGetLED(devAddress, btnPin))
			ButtonSetLED(devAddress, btnPin, 0);
		else
			ButtonSetLED(devAddress, btnPin, 1);
	}
	const char * state = isPressed ? "pressed" : "released";
	char * buttonName;
	switch (devAddress) {
	case IOEXP_ADDRESS_F:
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
			buttonName = "UNKNOWN BTN_F";
			break;
		}
		break;
	case IOEXP_ADDRESS_S:
		switch (btnPin) {
		case BTN_PIN_S1:
			buttonName = "BTN_S1";
			break;
		case BTN_PIN_S2:
			buttonName = "BTN_S2";
			break;
		case BTN_PIN_S3:
			buttonName = "BTN_S3";
			break;
		case BTN_PIN_S4:
			buttonName = "BTN_S4";
			break;
		default:
			buttonName = "UNKNOWN BTN_S";
			break;
		}
		break;
	default:
		buttonName = "UNKNOWN ADDR";
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
	const int BUTTONS_F[] = { BTN_PIN_F1, BTN_PIN_F2, BTN_PIN_F3, BTN_PIN_F4, BTN_PIN_F5, BTN_PIN_F6, BTN_PIN_F7, BTN_PIN_F8 };
	const int NUM_BUTTONS_F = 8;
	const int BUTTONS_S[] = { BTN_PIN_S1, BTN_PIN_S2, BTN_PIN_S3, BTN_PIN_S4 };
	const int NUM_BUTTONS_S = 4;

	int i;

	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET) {

		uint16_t flags, state;
		uint8_t isPressed;

		Button_I2C_Read(IOEXP_ADDRESS_F, INTFA, (uint8_t *) &flags, sizeof(flags));
		Button_I2C_Read(IOEXP_ADDRESS_F, IOEXP_GPIOA, (uint8_t *) &state, sizeof(state));
		for (i = 0; i < NUM_BUTTONS_F; i++) {
			if (ButtonChangedState(BUTTONS_F[i], flags, state, &isPressed)) {
				ButtonEvent(IOEXP_ADDRESS_F, BUTTONS_F[i], isPressed);
			}
		}

		flags = 0;
		state = 0;
		isPressed = 0;

		Button_I2C_Read(IOEXP_ADDRESS_S, INTFA, (uint8_t *) &flags, sizeof(flags));
		Button_I2C_Read(IOEXP_ADDRESS_S, IOEXP_GPIOA, (uint8_t *) &state, sizeof(state));
		for (i = 0; i < NUM_BUTTONS_S; i++) {
			if (ButtonChangedState(BUTTONS_S[i], flags, state, &isPressed)) {
				ButtonEvent(IOEXP_ADDRESS_S, BUTTONS_S[i], isPressed);
			}
		}
	}
	EXTI->PR |= B2_Pin;
	while (HAL_I2C_IsDeviceReady(btnHi2c, IOEXP_ADDRESS_F, 100, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	while (HAL_I2C_IsDeviceReady(btnHi2c, IOEXP_ADDRESS_S, 100, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET) {
		EXTI9_5_IRQHandler();
	}
}

void ButtonsInitIOexpander(uint16_t devAddress) {
	// Setup I/O expanders through I2C.

	// Byte mode with IOCON.BANK = 0, INT pins internally connected, INT active-high
	uint8_t settings = IOCON_MIRROR | IOCON_INTPOL;
	Button_I2C_Write(devAddress, IOCONA, &settings, sizeof(settings));
	Button_I2C_Write(devAddress, IOCONB, &settings, sizeof(settings));

	uint16_t direction = 0b1010101010101010; // Each even pin is output, odd inputs (1 = Pin is configured as an input)
	Button_I2C_Write(devAddress, IODIRA, (uint8_t *) &direction, sizeof(direction));

	// 1 = Enable GPIO input pin for interrupt-on-change event.
	Button_I2C_Write(devAddress, GPINTENA, (uint8_t *) &direction, sizeof(direction));

	// Interrupt on change for inputs. 0 = Pin value is compared against the previous pin value.
	uint16_t intcon = 0;
	Button_I2C_Write(devAddress, INTCONA, (uint8_t *) &intcon, sizeof(intcon));

	// 1 = Pull-up enabled
	uint16_t pullups = 0xFFFF;
	Button_I2C_Write(devAddress, GPPUA, (uint8_t *) &pullups, sizeof(pullups));

	uint16_t levels = 0;
	Button_I2C_Write(devAddress, IOEXP_GPIOA, (uint8_t *) &levels, sizeof(levels));

	// Read once to reset any interrupt related stuff
	volatile uint16_t result;
	Button_I2C_Read(devAddress, IOEXP_GPIOA, (uint8_t *) &result, sizeof(result));
}

void ButtonsInit(I2C_HandleTypeDef *hi2cHandle) {
	btnHi2c = hi2cHandle;
	ButtonsInitIOexpander(IOEXP_ADDRESS_S);
	ButtonsInitIOexpander(IOEXP_ADDRESS_F);
}
