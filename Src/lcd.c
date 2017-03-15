/*
 * lcd.c
 *
 *  Created on: Feb 21, 2017
 *      Author: user
 */

#include "lcd.h"
#include "SRP.h"

static TIM_HandleTypeDef *microSecondHtim;
static TIM_HandleTypeDef *pwmHtim;
static I2C_HandleTypeDef *lcdHi2c;

static volatile uint8_t targetBrightness;
static volatile uint16_t lStateA;
static volatile uint16_t lStateB;
static volatile int encPosition;

static volatile uint8_t sending;
static volatile uint8_t intPending;
static volatile uint8_t ledState;

void LCD_I2C_Write(uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Write(lcdHi2c, IOEXP_ADDRESS, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, LCD_I2C_TIMEOUT) != HAL_OK)
		;
}

void LCD_I2C_Read(uint16_t MemAddress, uint8_t *pData, uint16_t Size) {
	while (HAL_I2C_Mem_Read(lcdHi2c, IOEXP_ADDRESS, MemAddress, I2C_MEMADD_SIZE_8BIT, pData, Size, LCD_I2C_TIMEOUT) != HAL_OK)
		;
}

void LCD_TIM_IRQHandler(TIM_HandleTypeDef *htimHandle) {
	// Timer interrupt for changing the backlight brightness in steps
	uint8_t currentBrightness = pwmHtim->Instance->CCR1;
	if (currentBrightness < targetBrightness) {
		pwmHtim->Instance->CCR1 = currentBrightness + 1;
	} else if (currentBrightness > targetBrightness) {
		pwmHtim->Instance->CCR1 = currentBrightness - 1;
	} else {
		HAL_NVIC_DisableIRQ(LCD_TIM_IRQ);
	}
}

void LCDsetButtonLED(uint8_t state) {
	uint16_t gpioState;
	LCD_I2C_Read(IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
	gpioState = (state) ? gpioState | LCD_BUTTON_LED_Pin : gpioState & ~LCD_BUTTON_LED_Pin;
	LCD_I2C_Write(IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
	ledState = state;
}

uint8_t LCDgetButtonLED(void) {
	uint16_t gpioState;
	LCD_I2C_Read(IOEXP_GPIOA, (uint8_t *) &gpioState, sizeof(gpioState));
	ledState = (gpioState & LCD_BUTTON_LED_Pin) ? 1 : 0;
	return ledState;
}

void LCDsetBrightness(uint8_t percent) {
	// Set the brightness of backlight in percent
	targetBrightness = percent;
	pwmHtim->Instance->CCR1 = percent;
}

uint8_t LCDgetBrightness(void) {
	// Get the current backlight in percent
	return pwmHtim->Instance->CCR1;
}

void LCDfadeBrightness(uint8_t percent, uint8_t secondsFade) {
	// Fade the brightness to some percent during some seconds
	targetBrightness = percent;
	if (secondsFade == 0) {
		LCDsetBrightness(targetBrightness);
	} else {
		uint8_t currentBrightness = pwmHtim->Instance->CCR1;
		uint8_t steps = (targetBrightness > currentBrightness) ? targetBrightness - currentBrightness : currentBrightness - targetBrightness;
		microSecondHtim->Instance->ARR = 1000000 / (steps) * secondsFade;
		HAL_NVIC_EnableIRQ(LCD_TIM_IRQ);
		HAL_NVIC_SetPriority(LCD_TIM_IRQ, 5, 0);
	}
}

void LCDbuttonEvent(int btnPin, uint8_t isPressed) {
	switch (btnPin) {
	case LCD_BUTTON_Pin:
		if (isPressed) {
			if (LCDgetButtonLED())
				LCDsetButtonLED(0);
			else
				LCDsetButtonLED(1);
			printf("BTN_LCD pressed\r\n");
		} else {
			printf("BTN_LCD released\r\n");
		}
		break;
	case LCD_ROT_ENC_W_Pin:
		if (isPressed)
			printf("push pressed\r\n");
		else
			printf("push released\r\n");
		break;
	case LCD_ROT_ENC_CW:
		encPosition++;
		printf("encoder %d\r\n", encPosition);
		break;
	case LCD_ROT_ENC_CCW:
		encPosition--;
		printf("encoder %d\r\n", encPosition);
		break;
	default:
		break;
	}
}

void EXTI0_IRQHandler(void) {
	if (sending) {
		BARRIER_LOCK
		EXTI->PR |= B0_Pin;
		intPending = 1;
		BARRIER_UNLOCK
		return;
	}
	NVIC_DisableIRQ(EXTI0_IRQn);
	BARRIER_LOCK
	volatile uint16_t flags, state, aHigh, bHigh, aLow, bLow;
	// No need to check INTFB/GPIOB probably
	while (HAL_I2C_IsDeviceReady(lcdHi2c, IOEXP_ADDRESS, 100, LCD_I2C_TIMEOUT) != HAL_OK)
		;
	LCD_I2C_Read(INTFA, (uint8_t *) &flags, sizeof(flags));
	LCD_I2C_Read(INTCAPA, (uint8_t *) &state, sizeof(state));

	if (flags & LCD_BUTTON_Pin)
		LCDbuttonEvent(LCD_BUTTON_Pin, (state & LCD_BUTTON_Pin) ? 0 : 1);

	if (flags & LCD_ROT_ENC_W_Pin)
		LCDbuttonEvent(LCD_ROT_ENC_W_Pin, (state & LCD_ROT_ENC_W_Pin) ? 0 : 1);

	aHigh = state & LCD_ROT_ENC_A_Pin;
	bHigh = state & LCD_ROT_ENC_B_Pin;
	aLow = !aHigh;
	bLow = !bHigh;

	if (flags & LCD_ROT_ENC_A_Pin) {
		if (aLow && bHigh) {
			// Initial falling edge on A, tick for clockwise rotation
			LCDbuttonEvent(LCD_ROT_ENC_CW, 1);
			lStateA = state;

		} else if (aHigh && bLow) {
			// Rising edge on A, check if we missed the previous tick...
			if ((lStateA & LCD_ROT_ENC_A_Pin) || !(lStateA & LCD_ROT_ENC_B_Pin)) {
				LCDbuttonEvent(LCD_ROT_ENC_CW, 1);
				lStateA = state;
			} else {
				lStateA = 0;
			}
		}
	}
	if (flags & LCD_ROT_ENC_B_Pin) {
		if (bLow && aHigh) {
			// Initial falling edge on B, tick for counter clockwise rotation
			LCDbuttonEvent(LCD_ROT_ENC_CCW, 1);
			lStateB = state;

		} else if (bHigh && aLow) {
			// Rising edge on B, check if we missed the previous tick...
			if ((lStateB & LCD_ROT_ENC_B_Pin) || !(lStateB & LCD_ROT_ENC_A_Pin)) {
				LCDbuttonEvent(LCD_ROT_ENC_CCW, 1);
				lStateB = state;
			} else {
				lStateB = 0;
			}
		}
	}
	intPending = 0;
	while (HAL_I2C_IsDeviceReady(lcdHi2c, IOEXP_ADDRESS, 100, LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// Reset the I/O expander interrupt
	LCD_I2C_Read(IOEXP_GPIOA, (uint8_t *) &state, sizeof(state));
	while (HAL_I2C_IsDeviceReady(lcdHi2c, IOEXP_ADDRESS, 100, LCD_I2C_TIMEOUT) != HAL_OK)
		;
	// Reset the MCU interrupt
	EXTI->PR |= B0_Pin;
	NVIC_EnableIRQ(EXTI0_IRQn);
	BARRIER_UNLOCK
	if (HAL_GPIO_ReadPin(B0_GPIO_Port, B0_Pin) == GPIO_PIN_SET) {
		EXTI0_IRQHandler();
	}
}
void LCDsendBytes(uint8_t rs, uint8_t bytes) {
	BARRIER_LOCK
		// Sends a byte (blocking transfer) through I2C
	uint16_t data = (rs) ? LCD_RS_Pin : 0;
	data |= bytes << 8;
	data = (ledState) ? data | LCD_BUTTON_LED_Pin : data & ~LCD_BUTTON_LED_Pin;
	sending = 1;
	LCD_I2C_Write(IOEXP_GPIOA, (uint8_t *) &data, sizeof(data));
	sending = 0;
	if (intPending)
		EXTI0_IRQHandler();
	// There is no need to set the data pins twice more, this could be faster by only addressing GPIOA (E pin).
	// However, the LCD still needs time to settle between writes, so delay might be needed in that case?
	data |= LCD_E_Pin;
	data = (ledState) ? data | LCD_BUTTON_LED_Pin : data & ~LCD_BUTTON_LED_Pin;
	sending = 1;
	LCD_I2C_Write(IOEXP_GPIOA, (uint8_t *) &data, sizeof(data));
	sending = 0;
	if (intPending)
		EXTI0_IRQHandler();
	data &= ~LCD_E_Pin;
	data = (ledState) ? data | LCD_BUTTON_LED_Pin : data & ~LCD_BUTTON_LED_Pin;
	sending = 1;
	LCD_I2C_Write(IOEXP_GPIOA, (uint8_t *) &data, sizeof(data));
	sending = 0;
	if (intPending)
		EXTI0_IRQHandler();
	BARRIER_UNLOCK
}

void LCDsendCmd(uint8_t cmd) {
	LCDsendBytes(0, cmd);
}

void LCDsendChar(char character) {
	LCDsendBytes(1, character);
}

void LCDcursorPos(uint8_t row, uint8_t column) {
	// Set the position of the internal LCD cursor
	switch (row) {
	case 0:
		LCDsendCmd(0x80 + column);
		break;
	case 1:
		LCDsendCmd(0xC0 + column);
		break;
	case 2:
		LCDsendCmd(0x90 + column + 4);
		break;
	case 3:
		LCDsendCmd(0xD0 + column + 4);
		break;
	default:
		break;
	}
}

void LCDwrite(char *string) {
	// Write a string to the LCD
	while (*string)
		LCDsendChar(*string++);
}

void LCDclear(void) {
	// Clear the LCD. Should not be used each frame, since this will cause flickering.
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(4); // >3 ms
}

void LCDclearRow(uint8_t row) {
	// Clear a single row. Should not be used each frame, since this will cause flickering.
	LCDcursorPos(row, 0);
	int i;
	for (i = 0; i < 20; i++)
		LCDsendChar(' ');
}

void LCDinit(TIM_HandleTypeDef *microSecondHtimHandle, TIM_HandleTypeDef *pwmHtimHandle, I2C_HandleTypeDef *hi2cHandle) {

	lcdHi2c = hi2cHandle;

	// Setup the I/O expander
	// Byte mode with IOCON.BANK = 0, INT pins internally connected, INT active-high
	uint8_t settings = IOCON_MIRROR | IOCON_INTPOL;
	LCD_I2C_Write(IOCONA, &settings, sizeof(settings));
	LCD_I2C_Write(IOCONB, &settings, sizeof(settings));

	// 1 = Pin is input
	uint16_t direction = LCD_BUTTON_Pin | LCD_ROT_ENC_W_Pin | LCD_ROT_ENC_B_Pin | LCD_ROT_ENC_A_Pin;
	LCD_I2C_Write(IODIRA, (uint8_t *) &direction, sizeof(direction));

	// 1 = Enable GPIO input pin for interrupt-on-change event.
	LCD_I2C_Write(GPINTENA, (uint8_t *) &direction, sizeof(direction));

	// Interrupt on change for inputs. 0 = Pin value is compared against the previous pin value.
	uint16_t intcon = 0;
	LCD_I2C_Write(INTCONA, (uint8_t *) &intcon, sizeof(intcon));

	// 1 = Pull-up enabled
	uint16_t pullups = 0xFFFF;
	LCD_I2C_Write(GPPUA, (uint8_t *) &pullups, sizeof(pullups));

	uint16_t levels = 0;
	LCD_I2C_Write(IOEXP_GPIOA, (uint8_t *) &levels, sizeof(levels));

	// Read once to reset any interrupt related stuff
	volatile uint16_t result;
	LCD_I2C_Read(IOEXP_GPIOA, (uint8_t *) &result, sizeof(result));

	// Setup PWM to control backlight brightness
	pwmHtim = pwmHtimHandle;
	HAL_TIM_Base_Start(pwmHtim);
	HAL_TIM_PWM_Start(pwmHtim, LCD_PWM_CHANNEL);
	pwmHtim->Instance->ARR = 100; // Period
	pwmHtim->Instance->CCR1 = 0; // Pulse

	microSecondHtim = microSecondHtimHandle;
	HAL_TIM_Base_Init(microSecondHtim);
	HAL_TIM_Base_Start_IT(microSecondHtim);

	// Wait for LCD to power on fully, then set the correct mode
	HAL_Delay(200);
	LCDsendCmd(LCD_CMD_INIT);
	HAL_Delay(1);
	LCDsendCmd(LCD_CMD_ON_CURSOR_OFF);
	HAL_Delay(1);
	LCDsendCmd(LCD_CMD_CLEAR);
	HAL_Delay(5);
}
