/*========================================================
	
	Description: 
	
	Author: Sam Karimi
========================================================*/

#include "Controller.h"


// key bindings
// NOTE: Two different key binding maps for the two states of
// the program. The user button command generated will differ 
// depending on which state the program is running in.
usercmdkey_t usercmdKeysModeA[] = {
	{ slider_a,		ub_sliderA },
	{ slider_b,		ub_sliderB },
	{ slider_c,		ub_sliderC },
	{ slider_d,		ub_sliderD },
	
	
	// FIXME!
	
	
	{ key_null,		ub_none },
};

//
usercmdkey_t usercmdKeysModeB[] = {
	{ slider_a,		ub_sliderA },
	{ slider_b,		ub_sliderB },
	{ slider_c,		ub_sliderC },
	{ slider_d,		ub_sliderD },


	// FIXME!
	
	
	{ key_null,		ub_none },
};


#define USER_CMD_QUE_MAX 128

#define Mode_A 25638
#define Mode_B 78965

int controllerMode = 25638;

usercmd_t usercmdQue[128];

signed int usercmdQueSize = 0;

inputcmds_t newInputCmd;
inputcmds_t oldInputCmd;

/*======================================
	Private

	====================================
*/
void UserCmdQuePushBack( usercmd_t uCmd ) {
	if ( usercmdQueSize < USER_CMD_QUE_MAX ){
		usercmdQue[usercmdQueSize] = uCmd;
		usercmdQueSize++;
	}else {
		// drop user cmd if que is full
	}
}


/*======================================

	====================================
*/
void ControllerGenUserCmds() {
	// check if inputs cmds have changed
	
	
	// if cmd have changed generate user commands and buffer them in que

	
	
	// else no changes just return
	
}

/*======================================

	Description: Pops the front of the
	que.
	
	Time-Complexity: Linear
	====================================
*/
usercmd_t ControllerPopUserCmd() {
	
	usercmd_t uCmd;
	
	// check if que is empty
	if ( !usercmdQueSize ) {
		uCmd.button = ub_none;
		// return empty cmd
		return uCmd;
	}

	// else return front of que and shrink que size
	uCmd = usercmdQue[0];
	
	int i = 0;
	while ( 1 ) {
		if ( i == ( usercmdQueSize - 1) ) {
			usercmdQueSize--;
			break;
		}
		usercmdQue[i] = usercmdQue[i+1];
	}

	return uCmd;
}

/*========================================================
	

	======================================================
*/
void ControllerPollAnalogInput(){
	
}