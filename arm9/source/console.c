#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>
#include <string.h>

#include <ipc2.h>

#include "wheel.h"
#include "sound.h"
#include "file.h"
#include "playlist.h"

void displayconsole(void) {
	static u32 frame;
	static u32 musicsize;
	touchPosition touch;
	uint32 held;
	u32 wheel;

	held = keysHeld();
	touch = touchReadXY();

	iprintf("\x1b[0;0Htouch: (%3d,%3d)  %08X: keys\n", touch.px, touch.py, held);

	wheel = IPC2->wheel_status;

	if(wheel & WHEEL_RIGHT)
		iprintf("\x1b[1;0Hwheel: RIGHT  \n");
	else if(wheel & WHEEL_LEFT)
		iprintf("\x1b[1;0Hwheel: LEFT   \n");
	else if(wheel & WHEEL_MENU)
		iprintf("\x1b[1;0Hwheel: MENU   \n");
	else if(wheel & WHEEL_PLAY)
		iprintf("\x1b[1;0Hwheel: PLAY   \n");
	else if(wheel & WHEEL_FORWARD)
		iprintf("\x1b[1;0Hwheel: FORWARD\n");
	else if(wheel & WHEEL_REWIND)
		iprintf("\x1b[1;0Hwheel: REWIND \n");
	else if(wheel & WHEEL_CENTER)
		iprintf("\x1b[1;0Hwheel: CENTER \n");

	iprintf("\x1b[1;16H%9d: frame\n", frame++);

	iprintf("\x1b[2;0Hstate: %d\n", state);

	iprintf("\x1b[2;23H%d: format", format);

	if(IPC2->sound_state == IPC2_PLAYING) {
		iprintf("\x1b[3;0Hc: %d f: %5d b: %2d s: %4d", IPC2->sound_channels, IPC2->sound_frequency, IPC2->sound_bytes_per_sample * 8, IPC2->sound_samples);
	} else {
		iprintf("\x1b[3;0H                                ");
	}

	iprintf("\x1b[4;0Hbsz: %5d bsp: %5d blo: %5d\n", buffer_size, buffer_samples, buffer_lowest);

	if(strlen(currentmedia->path) != musicsize - 7) {
		int i, j;

		for(j = 0; j < musicsize/32 + 7; j++)
			for(i = 0; i < 32; i++)
				iprintf("\x1b[%d;%dH ", j + 5, i);

		musicsize = strlen(currentmedia->path) + 7;
	}

	iprintf("\x1b[5;0Hmusic: %s\n"
		"title: %s\n"
		"artist: %s\n"
		"album: %s\n",
		currentmedia->path,
		(currentmedia->title == NULL) ? "NULL" : currentmedia->title,
		(currentmedia->artist == NULL) ? "NULL" : currentmedia->artist,
		(currentmedia->album == NULL) ? "NULL" : currentmedia->album);

	if(IPC2->messageflag > 0) {
		static unsigned messagenumber;
		int i, j;

		for(j = 0; j < 1 + IPC2_MAX_MESSAGES * 3; j++)
			for(i = 0; i < 32; i++)
				iprintf("\x1b[%d;%dH ", 22 - (IPC2_MAX_MESSAGES + IPC2_MAX_MESSAGES2) * 3 - 1, i);

		iprintf("\x1b[%d;0H", 22 - (IPC2_MAX_MESSAGES + IPC2_MAX_MESSAGES2) * 3 - 1);
		if(IPC2->messageflag > IPC2_MAX_MESSAGES)
			printf("lost %d messages, ", IPC2->messageflag - 1);

		messagenumber += IPC2->messageflag;

		printf("message: %d\n", messagenumber);

		for(i = 0; i < IPC2->messageflag && i < IPC2_MAX_MESSAGES; i++)
			iprintf("\x1b[%d;0H%s\n", 22 - (IPC2_MAX_MESSAGES + IPC2_MAX_MESSAGES2) * 3 + 1 - 1 + i * 3, IPC2->message[i]);

		IPC2->messageflag = 0;
	}

	if(IPC2->messageflag2 > 0) {
		static unsigned messagenumber2;
		int i, j;

		for(j = 0; j < 1 + IPC2_MAX_MESSAGES2 * 3; j++)
			for(i = 0; i < 32; i++)
				iprintf("\x1b[%d;%dH ", 22 - IPC2_MAX_MESSAGES2 * 3, i);

		iprintf("\x1b[%d;0H", 22 - IPC2_MAX_MESSAGES2 * 3);
		if(IPC2->messageflag2 > IPC2_MAX_MESSAGES2)
			printf("lost %d messages, ", IPC2->messageflag2 - 1);

		messagenumber2 += IPC2->messageflag2;

		printf("message: %d\n", messagenumber2);

		for(i = 0; i < IPC2->messageflag2 && i < IPC2_MAX_MESSAGES2; i++)
			iprintf("\x1b[%d;0H%s\n", 22 - IPC2_MAX_MESSAGES2 * 3 + 1 + i * 3, IPC2->message2[i]);

		IPC2->messageflag2 = 0;
	}
}
