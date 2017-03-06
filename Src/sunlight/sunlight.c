#include "precompiled.h"
#include "sys_public.h"

#define MAX_PROGRAMS 	10
#define MAX_CHANNELS 	512

volatile uint16_t 		programs[MAX_PROGRAMS];
volatile uint16_t 		channels[MAX_CHANNELS];
char charBuffer[17];

typedef enum {
	prog_select,
	chan_select,
} modeSelect_t;

typedef struct {

	modeSelect_t	selectState;
	uint32_t 		currProgIndex;
	uint32_t 		currChanIndex;

} appState_t;

volatile appState_t sunState;

/*
 *
 */
void SunlightInit() {
	sunState.selectState 	= chan_select;
	sunState.currProgIndex 	= 0;
	sunState.currChanIndex 	= 0;

	int i = 0;
	for (i = 0; i < 512; i++) {
		channels[i] = 0;
		if (i < 10 )
			programs[i] = 0;
	}
}

void UpdateSunlightState( const usercmd_t * uCmd ) {

	switch( *uCmd ) {

	   case ub_select :
		   /*
		   if( sunState.selectState == prog_select ) {
			   sunState.selectState 	= chan_select;
			   sunState.currProgIndex 	= 0;
		   }
		   */
	      break;

	   case ub_back :
		   /*
		   if( sunState.selectState == chan_select ) {
			   sunState.selectState 	= prog_select;
			   sunState.currChanIndex 	= 0;
		   }
		   */
	      break;

	   case ub_up :
		   if( sunState.selectState == chan_select ) {
			   sunState.currChanIndex = ( ( sunState.currChanIndex + 4) % ( MAX_CHANNELS - 1 ) );	// wrap index on 511
		   }
	      break;

	   case ub_down :
		   if( sunState.selectState == chan_select ) {
			   sunState.currChanIndex -= 4;
			   if ( sunState.currChanIndex < 0 )
				   sunState.currChanIndex = MAX_CHANNELS - 5;	// index wrap to 507
		   }
	      break;

	   case ub_none :
		   // do nothing
	      break;
	}
}

/*
 *
 */
void UpdateOutput() {

	// FIXME! for now this only updates the channels

	// update lcd and dmx output
	int i;
	for (i = 0; i < 4; i++) {
		Dmx512SetChannelValue( channels[ sunState.currChanIndex + i], adcValues[i] );
		LCDcursorPos(i, 0);
		snprintf( charBuffer, 17, "Channel %03d: %03d", channels[ sunState.currChanIndex + i], adcValues[i] );
		LCDwrite( charBuffer );
	}
}

/*
 *
 */
void SunlightFrame(const usercmd_t * uCmd) {

	UpdateSunlightState( uCmd );

	// update lcd and dmx output
	UpdateOutput();

}
