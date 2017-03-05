/*========================================================
 
 Description: 
 
 Author: Sam Karimi
 ========================================================*/
#include "precompiled.h"
#include "sys_public.h"

// key bindings
// NOTE: Two different key binding maps for the two states of
// the program. The user button command generated will differ 
// depending on which state the program is running in.
volatile usercmdkey_t keyCmdMap[] = {
		{ 0, ub_select 	},
		{ 1, ub_back 	},
		{ 2, ub_left 	},
		{ 3, ub_right 	},
		{ 4, ub_none	},
		{ 5, ub_none	},
		{ 6, ub_none	},
		{ 7, ub_none	},
		{ 8, ub_none	},
		{ 9, ub_none	},
		{ 10, ub_none	},
		{ 11, ub_none	},
		{ 12, ub_none	},
		{ 13, ub_none	},
		{ 14, ub_none	},
		{ 15, ub_none	},
	};

#define MAX_CMD_BUF			60
#define MAX_KEYS			16							// number of physical buttons on this device

static volatile usercmd_t 	uCmdBuffer[MAX_CMD_BUF];
static volatile int 		uCmdIndex = 0;
static volatile int  		uCmdOutdex = 0;
volatile signed int 		uCmdBufSize = 0;

volatile uint32_t 			newKeyStates[16];
volatile uint32_t 			prevKeyStates[16];

/*
 *
 */
void ControllerInit(){
	int i;
	for (i = 0; i < 16; i++) {
		newKeyStates[i] = 0;
		prevKeyStates[i] = 0;
	}
}

/*
 *
 */
void UpdateKeyStates() {
	int i;
	for (i = 0; i < 16; i++) {
		newKeyStates[i] = keyStates[i];
	}
}

/*
 *
 */
int QueueCmd( usercmd_t uCmd ) {
	// Put a cmd in the FIFO
	if (uCmdIndex == ((uCmdOutdex - 1 + MAX_CMD_BUF) % MAX_CMD_BUF))
		return 0;
	uCmdBuffer[uCmdIndex] = uCmd;
	uCmdIndex = (uCmdIndex + 1) % MAX_CMD_BUF;
	return 1;
}

/*
 *
 */
void GenerateUserCmds() {
	int i;
	for ( i = 0; i < MAX_KEYS; i++ ){
		// check if keys have changed
		if ( newKeyStates[i] != keyStates[i] ) {
			// check which key and generate a user cmd
			usercmd_t cmd;
			cmd = keyCmdMap[i].button;
			QueueCmd(cmd);
		}
	}
}

/*
 *
 */
void ControllerGenUserCmds() {
	// retrieves and updates the key states
	UpdateKeyStates();

	// checks if keys have changed and generates and queues usercmd commands
	GenerateUserCmds();

	// set new key states as previous key states
	int i = 0;
	while (i != 16 ){
		prevKeyStates[i] = newKeyStates[i];
	}
}

/*
 *
 */
int ControllerPopUserCmd( usercmd_t * uCmd ) {
	// Pop a character from the FIFO
	if (uCmdIndex == uCmdOutdex)
		return 0;
	* uCmd = uCmdBuffer[uCmdOutdex];
	uCmdOutdex = (uCmdOutdex + 1) % MAX_CMD_BUF;
	return 1;
}

