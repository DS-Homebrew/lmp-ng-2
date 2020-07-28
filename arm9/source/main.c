#include <fat.h>
#include <nds.h>
#include <nds/arm9/console.h>
#include <stdio.h>

#include <ipc2.h>

#define POWER_OFF_VBLANKS 600

#include "screen.h"
#include "sound.h"
#include "playlist.h"
#include "input.h"

extern void displayconsole(void);
extern void displayconsoleinit(void);
extern void initdisplays(void);

int main(void) {
	unsigned power_state = 0, power_vblanks = POWER_OFF_VBLANKS;
	int lastkeys;
	int i;

	powerON(POWER_ALL_2D);

	irqInit();
	irqEnable(IRQ_VBLANK);

	// printf("starting fat...");
	if(fatInitDefault() != 1) {
		// printf("fatInitDefault() failed!\n");
		while(1)
			swiWaitForVBlank();
	}
	// printf("OK\n");

	screen_initdisplays();
	scanKeys();
	if(keysHeld() & KEY_START)
		videoSetMode(MODE_5_2D | DISPLAY_BG0_ACTIVE);

	printf("display ok!\n");
	printf("starting...\n");

	IPC2->messageflag = IPC2->messageflag2 = 0;
	input_hold = INPUT_HOLD_OFF;

	printf("starting sound...");
	IPC2->sound_volume = 0x3fff;
	IPC2->sound_control = 0;
	IPC2->sound_mode = IPC2_SOUND_MODE_PCM_NORMAL;
	state = STOPPED;
	format = UNKNOWN;
	sound_init();
	printf("OK\n");

	printf("scanning device for music files...");
	scanKeys();
	file_scan("/");
	printf("OK\n");

	printf("sorting files...");
	file_sort();
	printf("OK\n");

	printf("building Songs playlist...");
	playlist_add(playlist_build_songs());
	printf("OK\n");

	scanKeys();
	if(keysHeld() & KEY_B)
		printf("skippint playlists scan...");
	else {
		printf("scanning device for playlists...");
		file_scan_playlists("/");
		printf("OK\n");
	}

	srand(IPC->time.rtc.seconds + IPC->time.rtc.minutes * 60 + IPC->time.rtc.hours * 3600);

	playlist_shuffle(playlists.p[0]);
	shufflepos = playlist_add(shufflelist);

	currentmedia = media;
	screen_mainmenu();

	printf("entering main loop...\n");
	printf("\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n\n");

	while(1) {
		displayconsole();

		scanKeys();

		if((keysDown() & KEY_START) != 0) {
			static enum { CONSOLE, GRAPHICS } mode = GRAPHICS;

			if(mode == GRAPHICS) {
				videoSetMode(MODE_5_2D | DISPLAY_BG0_ACTIVE);
				mode = CONSOLE;
			} else {
				videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE
					| DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_1D_BMP);
				mode = GRAPHICS;
			}

			IPC2->sound_volume = 0x3fff;
		}
		if((keysDown() & KEY_SELECT) != 0) {
			// input_hold = (input_hold + 1) % 3 + 1;
			input_hold = 3 - input_hold;
			IPC2->wheel_status = 0;
		}

		screen_update();
		sound_update();

		if(keysHeld() != lastkeys
			|| keysHeld() & KEY_TOUCH
			|| keysHeld() & KEY_R
			|| keysHeld() & KEY_L) {
			lastkeys = keysHeld();
			power_vblanks = POWER_OFF_VBLANKS;
		} else if(power_vblanks > 0)
			power_vblanks--;

		if((power_vblanks == 0 || (keysHeld() & KEY_LID)) && power_state == 0) {
			power_state = 1;

			IPC_SendSync(IPC2_REQUEST_SET_BACKLIGHTS_OFF);
			swiWaitForVBlank(); /* HACK */
			IPC_SendSync(IPC2_REQUEST_LEDBLINK_ON);
			swiWaitForVBlank(); /* HACK */
		} else if(power_vblanks > 0 && (keysHeld() & KEY_LID) == 0 && power_state == 1) {
			power_state = 0;

			IPC_SendSync(IPC2_REQUEST_SET_BACKLIGHTS_ON);
			swiWaitForVBlank(); /* HACK */
			IPC_SendSync(IPC2_REQUEST_LEDBLINK_OFF);
			swiWaitForVBlank(); /* HACK */
		}

		swiWaitForVBlank();
	}

	return 0;
}
