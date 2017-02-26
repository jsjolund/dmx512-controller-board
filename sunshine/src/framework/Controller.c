/*========================================================
	
	Description: 
	
	Author: Sam Karimi
========================================================*/

#include "Controller.h"


// key bindings
// NOTE: Two different key binding maps for the two states of
// the program. The user button command generated will differ 
// depending on which state the program is running in.
volatile usercmdkey_t usercmdKeysModeA[] = {
	{ slider_a,		ub_sliderA },
	{ slider_b,		ub_sliderB },
	{ slider_c,		ub_sliderC },
	{ slider_d,		ub_sliderD },
	
	
	// FIXME!
	
	
	{ key_null,		ub_none },
};

//
volatile usercmdkey_t usercmdKeysModeB[] = {
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

volatile int controllerMode = 25638;

volatile usercmd_t usercmdQue[128];

volatile signed int usercmdQueSize = 0;

volatile slidercmds_t 	newSliders;		// slider states
volatile slidercmds_t 	oldSliders;		

volatile keycmds_t		newKeys;		// key states on the key card
volatile keycmds_t		oldKeys;		

volatile padcmds_t		newPadkeys;		// rotary key and back key states
volatile padcmds_t		oldPadkeys;		

/*======================================
	Private							   
									   
  ======================================
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

======================================*/
inline void ControllerGenUserCmds() {
	// check if inputs cmds have changed both for analog and i2c buttons		// this should have mutex lock on because inturrepts can set 
	
	
	// if cmd have changed generate user commands and buffer them in que

	
	
	// else no changes just return
	
}

/*======================================

	Description: Pops the front of the
	que.
	
	Time-Complexity: Linear
	====================================
*/
inline usercmd_t ControllerPopUserCmd() {
	
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
	

  ========================================================
*/
inline void ControllerPollAnalogInput(){
	
	
	
}