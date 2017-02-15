/*========================================================
	
	Description: 
	
	Author: Sam Karimi
========================================================*/
#include "winInput.h"
#include <conio.h>
#include <stdio.h>




void DetectInput() {
	int c = 0;

	switch( ( c = getch() ) ) {
		case KEY_UP:
		printf ("%s \n", "Up");				//key up
		break;
	case KEY_DOWN:
		//cout << endl << "Down" << endl;   // key down
		break;
	case KEY_LEFT:
		//cout << endl << "Left" << endl;  // key left
		break;
	case KEY_RIGHT:
		//cout << endl << "Right" << endl;  // key right
		break;
	case KEY_ONE:
		printf ("%s \n", "One");				//key up
		break;
	default:
		//cout << endl << "null" << endl;  // not arrow
		break;
	}
	
}

void BufferInputCmd() {
	
	
}