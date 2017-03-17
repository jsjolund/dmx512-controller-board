/*
 * buttons.c
 *
 *  Created on: Feb 27, 2017
 *      Author: user
 */

#include "buttons.h"

struct buttonStruct buttonMap[] = {

{ .btn = BTN_F1, .btnName = "BTN_F1", .btnPin = GPA1, .ledPin = GPA0, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F2, .btnName = "BTN_F2", .btnPin = GPA3, .ledPin = GPA2, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F3, .btnName = "BTN_F3", .btnPin = GPA5, .ledPin = GPA4, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F4, .btnName = "BTN_F4", .btnPin = GPA7, .ledPin = GPA6, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F5, .btnName = "BTN_F5", .btnPin = GPB7, .ledPin = GPB6, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F6, .btnName = "BTN_F6", .btnPin = GPB5, .ledPin = GPB4, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F7, .btnName = "BTN_F7", .btnPin = GPB3, .ledPin = GPB2, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_F8, .btnName = "BTN_F8", .btnPin = GPB1, .ledPin = GPB0, .ioexp_address = IOEXP_ADDRESS_F, .hi2c = 0 },

{ .btn = BTN_S1, .btnName = "BTN_S1", .btnPin = GPB1, .ledPin = GPB0, .ioexp_address = IOEXP_ADDRESS_S, .hi2c = 0 },

{ .btn = BTN_S2, .btnName = "BTN_S2", .btnPin = GPB3, .ledPin = GPB2, .ioexp_address = IOEXP_ADDRESS_S, .hi2c = 0 },

{ .btn = BTN_S3, .btnName = "BTN_S3", .btnPin = GPB5, .ledPin = GPB4, .ioexp_address = IOEXP_ADDRESS_S, .hi2c = 0 },

{ .btn = BTN_S4, .btnName = "BTN_S4", .btnPin = GPB7, .ledPin = GPB6, .ioexp_address = IOEXP_ADDRESS_S, .hi2c = 0 },

{ .btn = BTN_LCD, .btnName = "BTN_LCD", .btnPin = GPA1, .ledPin = GPA0, .ioexp_address = IOEXP_ADDRESS, .hi2c = 0 },

{ .btn = BTN_ENC, .btnName = "BTN_ENC", .btnPin = GPA2, .ledPin = -1, .ioexp_address = IOEXP_ADDRESS, .hi2c = 0 },

{ .btn = ENC_CW, .btnName = "ENC_CW", .btnPin = -1, .ledPin = -1, .ioexp_address = IOEXP_ADDRESS, .hi2c = 0 },

{ .btn = ENC_CCW, .btnName = "ENC_CCW", .btnPin = -1, .ledPin = -1, .ioexp_address = IOEXP_ADDRESS, .hi2c = 0 },

{ .btn = BTN_NONE, .btnName = "BTN_NONE", .btnPin = -1, .ledPin = -1, .ioexp_address = -1, .hi2c = 0 }

};

static I2C_HandleTypeDef *btnHi2c;
volatile uint8_t lcdButtonLEDstate;

void Button_I2C_Read(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Read(btnHi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}

void Button_I2C_Write(uint16_t DevAddress, uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Write(btnHi2c, DevAddress, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
}

void ButtonSetFNbuttonLEDS(uint8_t state) {
	NVIC_DisableIRQ(EXTI0_IRQn);
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	uint16_t gpioState;
	// Might as well hard code these values...
	while (HAL_I2C_Mem_Read(btnHi2c, IOEXP_ADDRESS_F, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	uint16_t pins = (buttonMap[BTN_F1].ledPin | buttonMap[BTN_F2].ledPin | buttonMap[BTN_F3].ledPin | buttonMap[BTN_F4].ledPin | buttonMap[BTN_F5].ledPin | buttonMap[BTN_F6].ledPin
			| buttonMap[BTN_F7].ledPin | buttonMap[BTN_F8].ledPin);
	if (state) {
		gpioState |= pins;
	} else {
		gpioState &= ~pins;
	}
	while (HAL_I2C_Mem_Write(btnHi2c, IOEXP_ADDRESS_F, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void ButtonSetLED(struct buttonStruct *button, uint8_t state) {
	NVIC_DisableIRQ(EXTI0_IRQn);
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	uint16_t gpioState;
	while (HAL_I2C_Mem_Read(button->hi2c, button->ioexp_address, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	if (state) {
		gpioState |= button->ledPin;
		if (button->btn == BTN_LCD)
			lcdButtonLEDstate = 1;
	} else {
		gpioState &= ~(button->ledPin);
		if (button->btn == BTN_LCD)
			lcdButtonLEDstate = 0;
	}
	while (HAL_I2C_Mem_Write(button->hi2c, button->ioexp_address, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
}

uint8_t ButtonGetLED(struct buttonStruct *button) {
	NVIC_DisableIRQ(EXTI0_IRQn);
	NVIC_DisableIRQ(EXTI9_5_IRQn);
	uint16_t gpioState;
	while (HAL_I2C_Mem_Read(button->hi2c, button->ioexp_address, IOEXP_GPIOA, I2C_MEMADD_SIZE_8BIT, (uint8_t *) &gpioState, sizeof(gpioState), BUTTONS_I2C_TIMEOUT) != HAL_OK)
		;
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	return (gpioState & button->ledPin) ? 1 : 0;
}

void ButtonEvent(struct buttonStruct *button, int isPressed) {
	ControllerQueueCmd(button->btn, isPressed);
}

uint8_t ButtonChangedState(int btnPin, uint16_t flags, uint16_t status, uint8_t *isPressed) {
	if (flags & btnPin) {
		*isPressed = (status & btnPin) ? BUTTON_RELEASED : BUTTON_PRESSED;
		return 1;
	}
	return 0;
}

void EXTI9_5_IRQHandler(void) {
	const int NUM_BUTTONS_F = 8;
	struct buttonStruct buttonsF[] = { buttonMap[BTN_F1], buttonMap[BTN_F2], buttonMap[BTN_F3], buttonMap[BTN_F4], buttonMap[BTN_F5], buttonMap[BTN_F6], buttonMap[BTN_F7], buttonMap[BTN_F8] };

	const int NUM_BUTTONS_S = 4;
	struct buttonStruct buttonsS[] = { buttonMap[BTN_S1], buttonMap[BTN_S2], buttonMap[BTN_S3], buttonMap[BTN_S4] };

	int i;

	if (HAL_GPIO_ReadPin(B2_GPIO_Port, B2_Pin) == GPIO_PIN_SET) {

		uint16_t flags, state;
		uint8_t isPressed;

		Button_I2C_Read(IOEXP_ADDRESS_F, INTFA, (uint8_t *) &flags, sizeof(flags));
		Button_I2C_Read(IOEXP_ADDRESS_F, IOEXP_GPIOA, (uint8_t *) &state, sizeof(state));
		for (i = 0; i < NUM_BUTTONS_F; i++) {
			if (ButtonChangedState(buttonsF[i].btnPin, flags, state, &isPressed)) {
				ButtonEvent(&buttonsF[i], isPressed);
			}
		}

		flags = 0;
		state = 0;
		isPressed = 0;

		Button_I2C_Read(IOEXP_ADDRESS_S, INTFA, (uint8_t *) &flags, sizeof(flags));
		Button_I2C_Read(IOEXP_ADDRESS_S, IOEXP_GPIOA, (uint8_t *) &state, sizeof(state));
		for (i = 0; i < NUM_BUTTONS_S; i++) {
			if (ButtonChangedState(buttonsS[i].btnPin, flags, state, &isPressed)) {
				ButtonEvent(&buttonsS[i], isPressed);
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

	buttonMap[BTN_F1].hi2c = hi2cHandle;
	buttonMap[BTN_F2].hi2c = hi2cHandle;
	buttonMap[BTN_F3].hi2c = hi2cHandle;
	buttonMap[BTN_F4].hi2c = hi2cHandle;
	buttonMap[BTN_F5].hi2c = hi2cHandle;
	buttonMap[BTN_F6].hi2c = hi2cHandle;
	buttonMap[BTN_F7].hi2c = hi2cHandle;
	buttonMap[BTN_F8].hi2c = hi2cHandle;
	buttonMap[BTN_S1].hi2c = hi2cHandle;
	buttonMap[BTN_S2].hi2c = hi2cHandle;
	buttonMap[BTN_S3].hi2c = hi2cHandle;
	buttonMap[BTN_S4].hi2c = hi2cHandle;

	ButtonsInitIOexpander(IOEXP_ADDRESS_S);
	ButtonsInitIOexpander(IOEXP_ADDRESS_F);
}
