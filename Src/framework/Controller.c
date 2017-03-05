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
volatile usercmdkey_t usercmdKeysModeA[] = {
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

		{ key_null, ub_none },
	};

#define USER_CMD_MAX 60

static volatile usercmd_t uCmdBuffer[USER_CMD_MAX];
static volatile int uCmdIndex = 0;
static volatile int  uCmdOutdex = 0;
volatile signed int uCmdBufSize = 0;

volatile uint32_t newKeyStates[16];
volatile uint32_t prevKeyStates[16];

void ControllerInit(){
	int i = 0;
	while (i != 16 ){
		newKeyStates[i] = 0;
		prevKeyStates[i] = 0;
	}
}

int QueueCmd( usercmd_t uCmd ) {
	// Put a character in the FIFO
	if (uCmdIndex == ((uCmdOutdex - 1 + USER_CMD_MAX) % USER_CMD_MAX))
		return 0;
	uCmdBuffer[uCmdIndex] = uCmd;
	uCmdIndex = (uCmdIndex + 1) % USER_CMD_MAX;
	return 1;
}

void UpdateKeyStates() {

}


inline void ControllerGenUserCmds() {

	updateKeyStates()


}

int ControllerPopUserCmd( usercmd_t * uCmd ) {
	// Pop a character from the FIFO
	if (uCmdIndex == uCmdOutdex)
		return 0;
	* uCmd = uCmdBuffer[uCmdOutdex];
	uCmdOutdex = (uCmdOutdex + 1) % USER_CMD_MAX;
	return 1;
}

