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
} usercmdButton_t;

//
typedef struct {
	uint32_t			key;
	usercmdButton_t 	button;
} usercmdkey_t;

typedef struct {
	usercmdButton_t button;
} usercmd_t;

ControllerInit();




void ControllerGenUserCmds();
int ControllerPopUserCmd( usercmd_t * uCmd );

#endif
