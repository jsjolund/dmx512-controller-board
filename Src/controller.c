#include "controller.h"

#define MAX_CMD_BUF	10

static enum buttonEnum uCmdBuffer[MAX_CMD_BUF];
static uint8_t uCmdState[MAX_CMD_BUF];
static int uCmdIndex = 0;
static int uCmdOutdex = 0;

static volatile uint8_t newDmxDataLoaded = 0;

static volatile int encPosition;

static volatile uint8_t selectedDmxProgram;
static volatile int8_t mainMenuCursor;
static volatile uint8_t mainMenuEditLCDbacklightMode;
static volatile uint8_t mainMenuProgramWasSaved;

int ControllerPopCmd(enum buttonEnum *button, int *buttonState) {
	// Pop a character from the FIFO
	if (uCmdIndex == uCmdOutdex)
		return 0;
	*button = uCmdBuffer[uCmdOutdex];
	*buttonState = uCmdState[uCmdOutdex];
	uCmdOutdex = (uCmdOutdex + 1) % MAX_CMD_BUF;
	return 1;
}

int ControllerQueueCmd(enum buttonEnum button, int buttonState) {
	// Put a cmd in the FIFO
	if (uCmdIndex == ((uCmdOutdex - 1 + MAX_CMD_BUF) % MAX_CMD_BUF))
		return 0;
	uCmdBuffer[uCmdIndex] = button;
	uCmdState[uCmdIndex] = buttonState;
	uCmdIndex = (uCmdIndex + 1) % MAX_CMD_BUF;
	return 1;
}

void ControlleSetSelectedDMXchannels(int steps) {
	selectedDmxChannels[0] = (((uint16_t) (selectedDmxChannels[0] + steps)) % 512);
	selectedDmxChannels[1] = (((uint16_t) (selectedDmxChannels[1] + steps)) % 512);
	selectedDmxChannels[2] = (((uint16_t) (selectedDmxChannels[2] + steps)) % 512);
	selectedDmxChannels[3] = (((uint16_t) (selectedDmxChannels[3] + steps)) % 512);
}

void ControllerLockDMXinput(void) {
	selectedDmxInputLock[0] = 1;
	ButtonSetLED(&buttonMap[BTN_S1], 1);
	selectedDmxInputLock[1] = 1;
	ButtonSetLED(&buttonMap[BTN_S2], 1);
	selectedDmxInputLock[2] = 1;
	ButtonSetLED(&buttonMap[BTN_S3], 1);
	selectedDmxInputLock[3] = 1;
	ButtonSetLED(&buttonMap[BTN_S4], 1);
}

void ControllerLoadDMXprogram(int program) {
	if (program == 0)
		return;
	ControllerLockDMXinput();
	EEPROMread(DMX_CHANNELS * program, tmpDmxData, DMX_CHANNELS);
	newDmxDataLoaded = 1;
}

void ControllerSaveDMXprogram(int program) {
	if (program == 0)
		return;
	memcpy(tmpDmxData, &dmxData[1], DMX_CHANNELS * sizeof(uint8_t));
	EEPROMwrite(DMX_CHANNELS * program, tmpDmxData, DMX_CHANNELS);
}

void ControllerEditProgram(enum buttonEnum button, int buttonState) {
	if (button == ENC_CW || button == ENC_CCW || button == BTN_ENC)
		ControllerLockDMXinput();

	if (buttonState == BUTTON_PRESSED) {

		switch (button) {
		case BTN_ENC:
			selectedDmxChannels[0] = 0;
			selectedDmxChannels[1] = 1;
			selectedDmxChannels[2] = 2;
			selectedDmxChannels[3] = 3;
			break;
		case ENC_CW:
			ControlleSetSelectedDMXchannels(4);
			break;
		case ENC_CCW:
			ControlleSetSelectedDMXchannels(-4);
			break;

		case BTN_F1:
			selectedDmxProgram = 1;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F1], 1);
			return;
		case BTN_F2:
			selectedDmxProgram = 2;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F2], 1);
			return;
		case BTN_F3:
			selectedDmxProgram = 3;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F3], 1);
			return;
		case BTN_F4:
			selectedDmxProgram = 4;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F4], 1);
			return;
		case BTN_F5:
			selectedDmxProgram = 5;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F5], 1);
			return;
		case BTN_F6:
			selectedDmxProgram = 6;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F6], 1);
			return;
		case BTN_F7:
			selectedDmxProgram = 7;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F7], 1);
			return;
		case BTN_F8:
			selectedDmxProgram = 8;
			ControllerLoadDMXprogram(selectedDmxProgram);
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F8], 1);
			return;

		case BTN_S1:
			selectedDmxInputLock[0] = (selectedDmxInputLock[0]) ? 0 : 1;
			if (selectedDmxInputLock[0])
				ButtonSetLED(&buttonMap[button], 1);
			else
				ButtonSetLED(&buttonMap[button], 0);
			break;
		case BTN_S2:
			selectedDmxInputLock[1] = (selectedDmxInputLock[1]) ? 0 : 1;
			if (selectedDmxInputLock[1])
				ButtonSetLED(&buttonMap[button], 1);
			else
				ButtonSetLED(&buttonMap[button], 0);
			break;
		case BTN_S3:
			selectedDmxInputLock[2] = (selectedDmxInputLock[2]) ? 0 : 1;
			if (selectedDmxInputLock[2])
				ButtonSetLED(&buttonMap[button], 1);
			else
				ButtonSetLED(&buttonMap[button], 0);
			break;
		case BTN_S4:
			selectedDmxInputLock[3] = (selectedDmxInputLock[3]) ? 0 : 1;
			if (selectedDmxInputLock[3])
				ButtonSetLED(&buttonMap[button], 1);
			else
				ButtonSetLED(&buttonMap[button], 0);
			break;
		default:
			break;
		}
	}
	LCDcursorPos(0, 0);
	LCDwrite("Chan Chan Chan Chan ");
	LCDcursorPos(1, 0);

	LCDcursorPos(1, 0);
	const int bufferLen = 21;
	char charBuffer[bufferLen];
	snprintf(charBuffer, bufferLen, " %03d  %03d  %03d  %03d ", selectedDmxChannels[0] + 1, selectedDmxChannels[1] + 1, selectedDmxChannels[2] + 1, selectedDmxChannels[3] + 1);
	LCDwrite(charBuffer);

	LCDcursorPos(2, 0);
	const char * locked = "LOCK";
	const char * open = "OPEN";
	snprintf(charBuffer, bufferLen, "%s %s %s %s ", selectedDmxInputLock[0] ? locked : open, selectedDmxInputLock[1] ? locked : open, selectedDmxInputLock[2] ? locked : open,
			selectedDmxInputLock[3] ? locked : open);
	LCDwrite(charBuffer);

	LCDcursorPos(3, 0);
	snprintf(charBuffer, bufferLen, " %03d  %03d  %03d  %03d ", selectedDmxInputLock[0] ? Dmx512GetChannelValue(selectedDmxChannels[0]) : adcValues[0],
			selectedDmxInputLock[1] ? Dmx512GetChannelValue(selectedDmxChannels[1]) : adcValues[1], selectedDmxInputLock[2] ? Dmx512GetChannelValue(selectedDmxChannels[2]) : adcValues[2],
			selectedDmxInputLock[3] ? Dmx512GetChannelValue(selectedDmxChannels[3]) : adcValues[3]);
	LCDwrite(charBuffer);
}

void ControllerAboutMenu(enum buttonEnum button, int buttonState) {
	LCDcursorPos(0, 0);
	LCDwrite("Daniel Brolin");
	LCDcursorPos(1, 0);
	LCDwrite("David Eriksson");
	LCDcursorPos(2, 0);
	LCDwrite("Johannes Sjolund");
	LCDcursorPos(3, 0);
	LCDwrite("Saman Karimi");
}

void ControllerUSARTpassthrough(enum buttonEnum button, int buttonState) {

}

void ControllerMenuProgram(enum buttonEnum button, int buttonState) {

	if (buttonState == BUTTON_PRESSED) {
		switch (button) {
		case BTN_ENC:
			if (mainMenuCursor == MENU_ITEM_SAVE_PROGRAM) {
				if (selectedDmxProgram > 0) {
					ControllerSaveDMXprogram(selectedDmxProgram);
				}
				mainMenuProgramWasSaved = 1;

			} else if (mainMenuCursor == MENU_ITEM_LCD_BACKLIGHT) {
				mainMenuEditLCDbacklightMode = (mainMenuEditLCDbacklightMode) ? 0 : 1;

			} else if (mainMenuCursor == MENU_ITEM_USART_PASSHTHROUGH) {
				ControllerLockDMXinput();
				passthroughMode = 1;
				prevControllerMenuFunction = controllerMenuFunction;
				controllerMenuFunction = &ControllerUSARTpassthrough;
				USB_USART->CR1 |= (USART_CR1_RXNEIE);

				LCDclear();
				LCDcursorPos(0, 0);
				LCDwrite("   PC USB control   ");
				LCDcursorPos(2, 0);
				LCDwrite("Emulating:");
				LCDcursorPos(3, 0);
				LCDwrite("Enttec Open DMX");
				return;

			} else if (mainMenuCursor == MENU_ITEM_ABOUT_SUNSHINE) {
				LCDclear();
				prevControllerMenuFunction = controllerMenuFunction;
				controllerMenuFunction = &ControllerAboutMenu;
				return;
			}
			break;
		case ENC_CW:
			if (mainMenuCursor == MENU_ITEM_LCD_BACKLIGHT && mainMenuEditLCDbacklightMode == 1) {
				int16_t brightness = LCDgetBrightness() + 5;
				if (brightness > 100)
					brightness = 100;
				LCDsetBrightness(brightness);
			} else {
				mainMenuCursor++;
				if (mainMenuCursor > MENU_CURSOR_MAX)
					mainMenuCursor = MENU_CURSOR_MAX;
			}
			break;
		case ENC_CCW:
			if (mainMenuCursor == 1 && mainMenuEditLCDbacklightMode == 1) {
				int16_t brightness = LCDgetBrightness() - 5;
				if (brightness < 0)
					brightness = 0;
				LCDsetBrightness(brightness);
			} else {
				mainMenuCursor--;
				if (mainMenuCursor < 0)
					mainMenuCursor = 0;
			}
			break;
		case BTN_F1:
			selectedDmxProgram = 1;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F1], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F2:
			selectedDmxProgram = 2;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F2], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F3:
			selectedDmxProgram = 3;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F3], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F4:
			selectedDmxProgram = 4;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F4], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F5:
			selectedDmxProgram = 5;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F5], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F6:
			selectedDmxProgram = 6;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F6], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F7:
			selectedDmxProgram = 7;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F7], 1);
			mainMenuProgramWasSaved = 0;
			break;
		case BTN_F8:
			selectedDmxProgram = 8;
			ButtonSetFNbuttonLEDS(0);
			ButtonSetLED(&buttonMap[BTN_F8], 1);
			mainMenuProgramWasSaved = 0;
			break;
		default:
			break;
		}
	}
	const int bufferLen = 21;
	char charBuffer[bufferLen];
	LCDcursorPos(MENU_ITEM_SAVE_PROGRAM, 0);
	LCDsendChar((mainMenuCursor == MENU_ITEM_SAVE_PROGRAM) ? 126 : ' ');
	if (mainMenuProgramWasSaved) {
		snprintf(charBuffer, bufferLen, " Program %03d saved ", selectedDmxProgram);
	} else if (selectedDmxProgram == 0) {
		snprintf(charBuffer, bufferLen, " Cannot save to 000");
	} else {
		snprintf(charBuffer, bufferLen, " Save program   %03d  ", selectedDmxProgram);
	}
	LCDwrite(charBuffer);

	LCDcursorPos(MENU_ITEM_LCD_BACKLIGHT, 0);
	LCDsendChar((mainMenuCursor == MENU_ITEM_LCD_BACKLIGHT) ? 126 : ' ');
	snprintf(charBuffer, bufferLen, " LCD Backlight  %03d", LCDgetBrightness());
	LCDwrite(charBuffer);

	LCDcursorPos(MENU_ITEM_USART_PASSHTHROUGH, 0);
	LCDsendChar((mainMenuCursor == MENU_ITEM_USART_PASSHTHROUGH) ? 126 : ' ');
	snprintf(charBuffer, bufferLen, " PC USB control    ");
	LCDwrite(charBuffer);

	LCDcursorPos(MENU_ITEM_ABOUT_SUNSHINE, 0);
	LCDsendChar((mainMenuCursor == MENU_ITEM_ABOUT_SUNSHINE) ? 126 : ' ');
	snprintf(charBuffer, bufferLen, " About Sunshine DMX");
	LCDwrite(charBuffer);
}

void ControllerUpdate(void) {
	if (!EEPROMfinished())
		return;
	else if (newDmxDataLoaded) {
		memcpy(&dmxData[1], tmpDmxData, DMX_CHANNELS * sizeof(uint8_t));
		newDmxDataLoaded = 0;
	}

	enum buttonEnum button = 0;
	int buttonState = 0;
	if (ControllerPopCmd(&button, &buttonState)) {

		if (button == BTN_LCD && buttonState == BUTTON_PRESSED) {
			mainMenuProgramWasSaved = 0;
			LCDclear();

			ControllerLockDMXinput();

			if (controllerMenuFunction == &ControllerAboutMenu) {
				controllerMenuFunction = prevControllerMenuFunction;
				prevControllerMenuFunction = &ControllerAboutMenu;

			} else if (controllerMenuFunction == &ControllerUSARTpassthrough) {
				USB_USART->CR1 &= ~(USART_CR1_RXNEIE);
				passthroughMode = 0;
				controllerMenuFunction = prevControllerMenuFunction;
				prevControllerMenuFunction = &ControllerAboutMenu;

			} else if (controllerMenuFunction == &ControllerMenuProgram) {
				prevControllerMenuFunction = controllerMenuFunction;
				controllerMenuFunction = &ControllerEditProgram;
				mainMenuEditLCDbacklightMode = 0;

			} else if (controllerMenuFunction == &ControllerEditProgram) {
				prevControllerMenuFunction = controllerMenuFunction;
				controllerMenuFunction = &ControllerMenuProgram;
			}
			(*controllerMenuFunction)(BTN_NONE, 0);

		} else {
			(*controllerMenuFunction)(button, buttonState);
		}

		if (!passthroughMode) {
			// Debug print to terminal
			if (button == ENC_CW) {
				encPosition++;
				printf("enc pos: %d\r\n", encPosition);

			} else if (button == ENC_CCW) {
				encPosition--;
				printf("enc pos: %d\r\n", encPosition);

			} else if (button != BTN_NONE) {
				const char * s = (buttonState == BUTTON_PRESSED) ? "pressed" : "released";
				printf("%s %s\r\n", buttonMap[button].btnName, s);
			}
		}

	} else {
		(*controllerMenuFunction)(BTN_NONE, 0);
	}

}
