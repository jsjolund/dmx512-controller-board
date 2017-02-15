/*========================================================
	File:			
	Description: 
	
	Author: Sam Karimi
========================================================*/



#include "Common.h"
#include "Controller.h"
#include "View.h"
#include "Dmx.h"

#include "../sunlight/sunlight.h"

/*======================================================
	Put things here that should init at startup

	====================================================
*/
void CommonInit(){
	
	
}

/*======================================================
	

	====================================================
*/
void CommonFrame() {

	while(1) {
		
		// (1) Input stage													// NOTE: the input stage should be run at around 100hz?
		// Poll Analog inputs
		ControllerPollAnalogInput();										// FIXME! doesnt do anything right now
		// Generate user commands
		ControllerGenUserCmds();											// NOTE: this should maybe have a mutex lock on the input 
																			//		 buffers if cpu decides to interuppt/update the input buffers.

		// (2) Sunshine stage												// NOTE: the sunshine frame should be run at around 30hz?
		usercmd_t uCmd;
		uCmd = ControllerPopUserCmd();
		// if we have a new user cmd run sunlight frame
		if ( uCmd.button != ub_none ) {
			//Send a input command to sunlight from the usercommand buffer.
			SunlightFrame( uCmd );											// NOTE: Sunlight have internal states which will trigger different functions.					
		}

		// (3) Update stage
		// Update dmx output
		DmxUpdate();
		
		// Update the LCD
		ViewUpdate();
    }
}