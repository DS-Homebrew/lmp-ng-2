#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <ipc2.h>

#include "wheel.h"

#define PEN_DOWN BIT(6)

static u16 wheel_status;

void scanwheel(void) {
	static int posx, posy;
	static int lastpx, lastpy;
	static int inwheel;
	static int moved;
	static int down;
	int px, py;
	static int held;
	uint16 buttons;

	int distance;

	touchPosition touch;

	buttons = REG_KEYXY;
	touch = touchReadXY();

	held = ~buttons;

	px = touch.px - CENTER_X;
	py = touch.py - CENTER_Y;

	if(down == 0 && held & PEN_DOWN) {
		lastpx = posx = px;
		lastpy = posy = py;

		down = 1;
		moved = 0;
	}

	distance = px * px + py * py;

	if(held & PEN_DOWN) {

		if(distance > 80*80 || distance < 30*30)
			inwheel = 0;
		else if(inwheel == 0) {
			inwheel = 1;
			posx = px;
			posy = py;
		} else {
			float angle;

			angle = atan2( (float) py, (float) px) - atan2( (float) posy, (float) posx);
			if(angle > 8*M_PI/9)
				angle -= 2*M_PI;
			if(angle < -8*M_PI/9)
				angle += 2*M_PI;

			if(angle > M_PI/9) {
				posx = px;
				posy = py;

				moved = 1;

				wheel_status |= WHEEL_RIGHT;
			} else if(angle < -M_PI/9) {
				posx = px;
				posy = py;

				moved = 1;

				wheel_status |= WHEEL_LEFT;
			}
		}
	} else { /* touch released */
		if(!moved && down == 1) {
			if(abs(posx) < 30 && abs(posy) < 30 && abs(lastpx) < 30 && abs(lastpy) < 30)
				wheel_status |= WHEEL_CENTER;
			else if(abs(posx - MENU_X) < 20 && abs(posy - MENU_Y) < 20 && abs(lastpx - MENU_X) < 20 && abs(lastpy - MENU_Y) < 20)
				wheel_status |= WHEEL_MENU;
			else if(abs(posx - FORWARD_X) < 20 && abs(posy - FORWARD_Y) < 20 && abs(lastpx - FORWARD_X) < 20 && abs(lastpy - FORWARD_Y) < 20)
				wheel_status |= WHEEL_FORWARD;
			else if(abs(posx - REWIND_X) < 20 && abs(posy - REWIND_Y) < 20 && abs(lastpx - REWIND_X) < 20 && abs(lastpy - REWIND_Y) < 20)
				wheel_status |= WHEEL_REWIND;
			else if(abs(posx - PLAY_X) < 20 && abs(posy - PLAY_Y) < 20 && abs(lastpx - PLAY_X) < 20 && abs(lastpy - PLAY_Y) < 20)
				wheel_status |= WHEEL_PLAY;
		}

		inwheel = 0;
		down = 0;
	}

	lastpx = px;
	lastpy = py;
}

u16 readwheel(void) {
	u16 status = wheel_status;

	wheel_status = 0;

	return status;
}
