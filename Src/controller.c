/*========================================================

 Description:

 Author: Sam Karimi
 ========================================================*/

#include "controller.h"

#define MAX_CMD_BUF	20

static enum buttonEnum uCmdBuffer[MAX_CMD_BUF];
static uint8_t uCmdState[MAX_CMD_BUF];
static int uCmdIndex = 0;
static int uCmdOutdex = 0;

static volatile int encPosition;

int ControllerPopCmd(enum buttonEnum *uCmd, int *state) {
	// Pop a character from the FIFO
	if (uCmdIndex == uCmdOutdex)
		return 0;
	*uCmd = uCmdBuffer[uCmdOutdex];
	*state = uCmdState[uCmdOutdex];
	uCmdOutdex = (uCmdOutdex + 1) % MAX_CMD_BUF;
	return 1;
}

int ControllerQueueCmd(enum buttonEnum uCmd, int state) {
	// Put a cmd in the FIFO
	if (uCmdIndex == ((uCmdOutdex - 1 + MAX_CMD_BUF) % MAX_CMD_BUF))
		return 0;
	uCmdBuffer[uCmdIndex] = uCmd;
	uCmdState[uCmdIndex] = state;
	uCmdIndex = (uCmdIndex + 1) % MAX_CMD_BUF;
	return 1;
}

void ControlleSetSelectedDMXchannels(int steps) {
	selectedDmxChannels[0] = (((uint16_t) (selectedDmxChannels[0] + steps)) % 512);
	selectedDmxChannels[1] = (((uint16_t) (selectedDmxChannels[1] + steps)) % 512);
	selectedDmxChannels[2] = (((uint16_t) (selectedDmxChannels[2] + steps)) % 512);
	selectedDmxChannels[3] = (((uint16_t) (selectedDmxChannels[3] + steps)) % 512);
}

void ControllerUpdate(void) {
	enum buttonEnum button = 0;
	int state = 0;
	if (ControllerPopCmd(&button, &state)) {

//		if (state == BUTTON_PRESSED && buttonMap[button].ledPin != -1) {
//			if (ButtonGetLED(&buttonMap[button]))
//				ButtonSetLED(&buttonMap[button], 0);
//			else
//				ButtonSetLED(&buttonMap[button], 1);
//		}

		if (button == ENC_CW) {
			ControlleSetSelectedDMXchannels(4);
			encPosition++;
			printf("enc pos: %d\r\n", encPosition);

		} else if (button == ENC_CCW) {
			ControlleSetSelectedDMXchannels(-4);
			encPosition--;
			printf("enc pos: %d\r\n", encPosition);

		} else {

			if (button == BTN_LCD) {
				uint8_t brightness = LCDgetBrightness();
				if (brightness == 100)
					LCDfadeBrightness(0, 3);
				else if (brightness == 0)
					LCDfadeBrightness(100, 3);
			}

			if (button == BTN_S1 && state == BUTTON_PRESSED) {
				selectedDmxChannelsLock[0] = (selectedDmxChannelsLock[0]) ? 0 : 1;
				if (selectedDmxChannelsLock[0])
					ButtonSetLED(&buttonMap[button], 1);
				else
					ButtonSetLED(&buttonMap[button], 0);

			} else if (button == BTN_S2 && state == BUTTON_PRESSED) {
				selectedDmxChannelsLock[1] = (selectedDmxChannelsLock[1]) ? 0 : 1;
				if (selectedDmxChannelsLock[1])
					ButtonSetLED(&buttonMap[button], 1);
				else
					ButtonSetLED(&buttonMap[button], 0);

			} else if (button == BTN_S3 && state == BUTTON_PRESSED) {
				selectedDmxChannelsLock[2] = (selectedDmxChannelsLock[2]) ? 0 : 1;
				if (selectedDmxChannelsLock[2])
					ButtonSetLED(&buttonMap[button], 1);
				else
					ButtonSetLED(&buttonMap[button], 0);

			} else if (button == BTN_S4 && state == BUTTON_PRESSED) {
				selectedDmxChannelsLock[3] = (selectedDmxChannelsLock[3]) ? 0 : 1;
				if (selectedDmxChannelsLock[3])
					ButtonSetLED(&buttonMap[button], 1);
				else
					ButtonSetLED(&buttonMap[button], 0);
			}

			const char * s = (state == BUTTON_PRESSED) ? "pressed" : "released";
			printf("%s %s\r\n", buttonMap[button].btnName, s);

		}
	}
	LCDcursorPos(0, 0);
	LCDwrite("Chan Chan Chan Chan ");
	LCDcursorPos(1, 0);

	LCDcursorPos(1, 0);
	const int bufferLen = 21;
	char charBuffer[bufferLen];
	snprintf(charBuffer, bufferLen, " %03d  %03d  %03d  %03d ", selectedDmxChannels[0], selectedDmxChannels[1],
			selectedDmxChannels[2], selectedDmxChannels[3]);
	LCDwrite(charBuffer);

	LCDcursorPos(2, 0);
	const char * locked = "LOCK";
	const char * open = "OPEN";
	snprintf(charBuffer, bufferLen, "%s %s %s %s ", selectedDmxChannelsLock[0] ? locked : open,
			selectedDmxChannelsLock[1] ? locked : open, selectedDmxChannelsLock[2] ? locked : open,
			selectedDmxChannelsLock[3] ? locked : open);
	LCDwrite(charBuffer);

	LCDcursorPos(3, 0);
	snprintf(charBuffer, bufferLen, " %03d  %03d  %03d  %03d ",
			selectedDmxChannelsLock[0] ? Dmx512GetChannelValue(selectedDmxChannels[0]) : adcValues[0],
			selectedDmxChannelsLock[1] ? Dmx512GetChannelValue(selectedDmxChannels[1]) : adcValues[1],
			selectedDmxChannelsLock[2] ? Dmx512GetChannelValue(selectedDmxChannels[2]) : adcValues[2],
			selectedDmxChannelsLock[3] ? Dmx512GetChannelValue(selectedDmxChannels[3]) : adcValues[3]);
	LCDwrite(charBuffer);
}
