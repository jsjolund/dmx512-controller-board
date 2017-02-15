/*========================================================
	
	Description: 
	
	Author: Sam Karimi
========================================================*/

#ifndef CONTROLLER_H
#define CONTROLLER_H

// NOTE: these are set to char values right 
// now because getch() fetches an char value from screen.
typedef enum {
	slider_a,
	slider_b,
	slider_c,
	slider_d,
//-----------------
	key_1 	= 0x31,
	key_2 	= 0x32,
	key_3 	= 0x33,
	key_4 	= 0x34,
	key_5 	= 0x35,
	key_6 	= 0x36,
	key_7 	= 0x37,
	key_8 	= 0x38,
	key_9 	= 0x71,		// q
	key_10 	= 0x77, 	// w
	key_11 	= 0x65,		// e
	key_12 	= 0x72,		// r
//-----------------
	rotary_left,
	rotary_right,
	rotary_down,
	key_back,
	key_null = 0
} key_t;

//
typedef enum { 
	state_down, 
	state_up,
	state_null = 0
} keystate_t;

//
typedef struct {
	key_t 		key;
	keystate_t 	keyState;
	int    		val;
} keyinput_t;

//
typedef struct {
	keyinput_t sliderA;
	keyinput_t sliderB;	
	keyinput_t sliderC;
	keyinput_t sliderD;
} slidercmds_t;

//
typedef struct {
	keyinput_t key1;
	keyinput_t key2;
	keyinput_t key3;
	keyinput_t key4;
	keyinput_t key5;
	keyinput_t key6;
	keyinput_t key7;
	keyinput_t key8;
	keyinput_t key9;
	keyinput_t key10;
	keyinput_t key11;
	keyinput_t key12;
} keycmds_t;

//
typedef struct {
	keyinput_t rotaryLeft;
	keyinput_t rotaryRight;
	keyinput_t rotaryDown;
	keyinput_t keyBack;
} padcmds_t;

//
typedef struct  {
	slidercmds_t 	sliders;	// slider states
	keycmds_t		keys;		// key states on the key card
	padcmds_t		padkeys;	// rotary key and back key states
} inputcmds_t;

// all user commands available for entire program
typedef enum {
	ub_select,
	ub_back,
	ub_sliderA,
	ub_sliderB,
	ub_sliderC,
	ub_sliderD,
	
	ub_none
} usercmdButton_t;

//
typedef struct {
	key_t			key;
	usercmdButton_t	button;
} usercmdkey_t;


typedef struct {
	usercmdButton_t button;
	int 			toggle;		// only 1 or 0
	int		 		val;		// val for value butons like the analog sliders
} usercmd_t;





//
void ControllerPollAnalogInput();

//
void BufferInputCmds();

//
void GenInputCmds();

void ControllerGenUserCmds();
usercmd_t ControllerPopUserCmd();

#endif