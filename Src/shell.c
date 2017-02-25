#include "shell.h"

void ShellExecute(char* string) {
	// Parse a user command
	char buffer[101];
	sprintf(buffer, string);
	char *token = strtok(buffer, " \t");

	uint8_t fail = 1;

	if (strncmp(token, "dmx", 3) == 0) {
		char *strChn = strtok(NULL, " \t");
		char *strVal = strtok(NULL, " \t");
		char *next = strtok(NULL, " \t");
		if (strChn && strVal && !next) {
			int channel = atoi(strChn);
			int value = atoi(strVal);
			if (channel < 512 && value < 256) {
				printf("Setting channel %d to %d\r\n", channel, value);
				Dmx512SetChannelValue(channel, value);
				fail = 0;
			} else {
				printf("Maximum values exceeded\r\n");
				return;
			}
		}

	} else if (strncmp(token, "brightness", 10) == 0) {
		char *strVal = strtok(NULL, " \t");
		char *next = strtok(NULL, " \t");
		if (strVal && !next) {
			int value = atoi(strVal);
			if (value >= 0 && value <= 100) {
				LCDfadeBrightness(value, 2);
				fail = 0;
			} else {
				printf("Invalid brightness\r\n");
				return;
			}
		}
	}
	if (fail) {
		printf("ERROR, could not parse \"%s\"\r\n", string);
	}
}
