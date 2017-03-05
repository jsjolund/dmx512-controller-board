/*========================================================
 
 Description: 
 
 Author: Sam Karimi
 ========================================================*/

#ifndef CONTROLLER_H
#define CONTROLLER_H


// all user commands available for entire program
typedef enum {
	ub_select,
	ub_back,
	ub_left,
	ub_right,


	ub_none
} usercmd_t;

//
typedef struct {
	int					key;
	usercmd_t 			button;
} usercmdkey_t;


void ControllerInit();




void ControllerGenUserCmds();
int ControllerPopUserCmd( usercmd_t * uCmd );

#endif
