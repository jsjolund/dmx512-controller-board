/*========================================================
 
 Description: 
 
 Author: Sam Karimi
 ========================================================*/

#include "precompiled.h"
#include "sys_public.h"

/*========================================================
 Description: This is where the logic happens.
 React to a incoming user user command 
 and generates Dmx and View commands.
 ======================================================
 */

volatile uint32_t CurrChannelIndex = 0;
volatile uint16_t Channels[512];
char charBuffer[17];

void SunlightInit() {

	int i = 0;
	while(i != 512 ) {
		Channels[i] = 0;
	}

}

void SunlightFrame(const usercmd_t * uCmd) {



	int i = 0;
	for (i = 0; i < 4; i++) {
		Dmx512SetChannelValue(Channels[ CurrChannelIndex + i], adcValues[i]);
		LCDcursorPos(i, 0);
		snprintf(charBuffer, 17, "Channel %03d: %03d", Channels[ CurrChannelIndex + i], adcValues[i] );
		LCDwrite(charBuffer);
	}

}
