/*========================================================
 File:			
 Description: 
 
 Author: Sam Karimi
 ========================================================*/
#include "precompiled.h"

/*======================================================
 Put things here that should init at startup

 ====================================================
 */
void CommonInit() {

}

/*======================================================
 

 ====================================================
 */
void CommonFrame() {

	// Generate user commands
	ControllerGenUserCmds();								// FIXME! this should maybe have a mutex lock on the input 
															//		 buffers if cpu decides to interuppt/update the input buffers.

	// (2) Sunshine stage												// FIXME! the sunshine frame should be run at around 100hz?
	usercmd_t uCmd;
	uCmd = ControllerPopUserCmd();
	// if we have a new user cmd run sunlight frame
	if (uCmd.button != ub_none) {
		//Send a input command to sunlight from the usercommand buffer.
		SunlightFrame(uCmd);// FIXME! Sunlight have internal states which will trigger different functions.					
	}

	// (3) Update stage
	// Update dmx output												// FIXME! both DMx and LCD should be updated in a certain hz
	DmxUpdate();

	// Update the LCD
	ViewUpdate();

}
