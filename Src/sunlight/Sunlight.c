/*========================================================
 
 Description: 
 
 Author: Sam Karimi
 ========================================================*/

#include "precompiled.h"

/*========================================================
 Description: This is where the logic happens.
 React to a incoming user user command 
 and generates Dmx and View commands.
 ======================================================
 */
void SunlightFrame(const usercmd_t uCmd) {
	if (uCmd.button != ub_none) {
		// nothing to update return
		return;
	}
	// update and set new states based on incoming user command.

	// Generate DMX cmd

	// Generate LCD cmd

}
