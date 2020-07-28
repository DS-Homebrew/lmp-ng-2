/*
 * ipc2.h
 *
 * Based on ipc.h in libnds which is
	Copyright (C) 2005
		Michael Noland (joat)
		Jason Rogers (dovoto)
		Dave Murphy (WinterMute)
	*
	*/

#ifndef NDS_IPC2_INCLUDE
#define NDS_IPC2_INCLUDE

#define IPC2_SOUND_START 1
#define IPC2_SOUND_STOP 2
#define IPC2_SOUND_CLICK 3

/* from ARM7 to ARM9 */
#define IPC2_REQUEST_WRITE_SOUND 1

/* from ARM9 to ARM7 */
#define IPC2_REQUEST_START_PLAYING 1
#define IPC2_REQUEST_STOP_PLAYING 2
#define IPC2_REQUEST_SET_BACKLIGHTS_OFF 3
#define IPC2_REQUEST_SET_BACKLIGHTS_ON 4
#define IPC2_REQUEST_LEDBLINK_OFF 5
#define IPC2_REQUEST_LEDBLINK_ON 6

#define IPC2_STOPPED 1
#define IPC2_PLAYING 2

#define IPC2_SOUND_MODE_PCM_OVERSAMPLING4x 1
#define IPC2_SOUND_MODE_PCM_OVERSAMPLING2x 2
#define IPC2_SOUND_MODE_PCM_NORMAL 3

#define IPC2_MAX_MESSAGES 1
#define IPC2_MAX_MESSAGES2 1

#include <nds/ipc.h>

typedef struct sTransferRegion2 {
	void *sound_lbuf, *sound_rbuf;

	u8 sound_control;
	u8 sound_mode;
	u8 sound_state;

	u8 sound_channels;
	u16 sound_frequency;
	u8 sound_bytes_per_sample;
	u16 sound_samples;

	u8 sound_writerequest;

	u16 wheel_status;

	char message[1][96];
	u16 messageflag;
	char message2[1][96];
	u16 messageflag2;

	u16 sound_volume;
	s16 **sound_tables;

	u8 sound_arm7_mode;
} TransferRegion2, * pTransferRegion2;


#define IPC2 ((TransferRegion2 volatile *)(0x027FF000+sizeof(TransferRegion)))

#endif
