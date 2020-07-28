#include <mad.h>

#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define SOUND_NEED_BUFFER

#include "sound.h"
#include "file.h"

#define INPUT_BUFFER_SIZE 8192
#define FILE_BUFFER_SIZE 8192

#define MAD_DECODED_BUFFER_SIZE 4096

#define MIN(A,B) ((A > B) ? (B) : (A))

static s16 *readbufL, *readbufR;
static int lastbuf, lastpos;

static struct mad_stream Stream;
static struct mad_frame Frame;
static struct mad_synth Synth;
//static mad_timer_t Timer;
static unsigned char *InputBuffer, *GuardPtr;
// static unsigned FrameCount;
static u32 mad_size;

static char *buffer;
static u32 buflen = 0;

u32 madplay_position(void);

void madplay_stop(void) {
	mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);

	fclose(sound_playfile);

	free(readbufL);
	free(readbufR);
	free(buffer);
	free(InputBuffer);
}

static s16 madplay_fixedtos16(mad_fixed_t sample) {
	sample += (1L << (MAD_F_FRACBITS - 16));

	/* Clipping */
	if(sample > MAD_F_ONE - 1)
		sample = MAD_F_ONE - 1;
	if(sample < -MAD_F_ONE)
		sample = MAD_F_ONE;

	/* Conversion. */
	return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static int madplay_read(void *ptr, u32 size) {
	u32 read;

	if(buflen < FILE_BUFFER_SIZE) {
		buflen += fread(buffer + buflen, 1, FILE_BUFFER_SIZE - buflen, sound_playfile);
	}

	if(buflen < size) {
		memcpy(ptr, buffer, buflen);
		read = buflen;
		buflen = 0;
	} else {
		memcpy(ptr, buffer, size);
		memmove(buffer, buffer + size, FILE_BUFFER_SIZE - size);
		read = size;
		buflen -= size;
	}

	if(buflen < FILE_BUFFER_SIZE) {
		buflen += fread(buffer + buflen, 1, FILE_BUFFER_SIZE - buflen, sound_playfile);
	}

	return read;
}

static int madplay_eof(void) {
	if(buflen == 0)
		return feof(sound_playfile);
	else
		return 0;
}

static int madplay_decode(void) {
	int i;

	if(state == FINISHING)
		return 0;

	if(Stream.buffer == NULL || Stream.error == MAD_ERROR_BUFLEN) {
		size_t ReadSize, Remaining;
		unsigned char *ReadStart;

		if(Stream.next_frame != NULL) {
			Remaining = Stream.bufend - Stream.next_frame;
			memmove(InputBuffer, Stream.next_frame, Remaining);
			ReadStart = InputBuffer + Remaining;
			ReadSize = INPUT_BUFFER_SIZE - Remaining;
		} else {
			ReadSize = INPUT_BUFFER_SIZE;
			ReadStart = InputBuffer;
			Remaining = 0;
		}

		madplay_read(ReadStart, ReadSize);
		if(madplay_eof()) {
			GuardPtr = ReadStart + ReadSize;
			memset(GuardPtr, 0, MAD_BUFFER_GUARD);
			ReadSize += MAD_BUFFER_GUARD;

			state = FINISHING;
		}

		mad_stream_buffer(&Stream, InputBuffer, ReadSize + Remaining);
		Stream.error = 0;
	}

	if(mad_frame_decode(&Frame, &Stream)) {
		if(MAD_RECOVERABLE(Stream.error)) {
			if(Stream.error != MAD_ERROR_LOSTSYNC || Stream.this_frame != GuardPtr) {
				if(IPC2->messageflag < IPC2_MAX_MESSAGES)
					sprintf(IPC2->message[IPC2->messageflag++], "recoverable frame level error (%s)\n", mad_stream_errorstr(&Stream));
			}
			return 1;
		} else {
			if(Stream.error == MAD_ERROR_BUFLEN)
				return 1;
			else {
				if(IPC2->messageflag < IPC2_MAX_MESSAGES)
					sprintf(IPC2->message[IPC2->messageflag++], "unrecoverable frame level error (%s).\n", mad_stream_errorstr(&Stream));

				return -1;
			}
		}
	}

//	FrameCount++;
	
//	mad_timer_add(&Timer, Frame.header.duration);

	mad_synth_frame(&Synth, &Frame);

	for(i=0; i < Synth.pcm.length; i++) {

		readbufL[i] = madplay_fixedtos16(Synth.pcm.samples[0][i]);

		if(MAD_NCHANNELS(&Frame.header)==2)
			readbufR[i] = madplay_fixedtos16(Synth.pcm.samples[1][i]);
	}

	lastbuf += Synth.pcm.length;

	return 0;
}

void madplay_update(void) {
	int oldval;
	int status;

	s16 *src16L, *dst16L, *src16R, *dst16R;
	int size;

	while(buffer_samples < buffer_size) {
		if(lastbuf == 0) {
			while((status = madplay_decode()) > 0);
			if(status < 0) {
				madplay_stop();
				state = ERROR;
				return;
			}

			if(state == FINISHING)
				return;

			lastpos = 0;
			sound_channels = MAD_NCHANNELS(&Frame.header);
			sound_samplerate =  Frame.header.samplerate;
			sound_elapsed = madplay_position() / (Frame.header.bitrate / 8);
			sound_time = mad_size / (Frame.header.bitrate / 8);
		}

		src16L = ((s16 *) readbufL) + lastpos;
		dst16L = ((s16 *) pcmbufL) + buffer_end;
		src16R = ((s16 *) readbufR) + lastpos;
		dst16R = ((s16 *) pcmbufR) + buffer_end;

		size = MIN(buffer_size - buffer_end, buffer_size - buffer_samples);
		size = MIN(size, lastbuf);

		/* we can't be interrupted here or the sound will be corrupted
		 * this shold be very fast, we're only copying the buffers */
		oldval = REG_IME;
		REG_IME = 0;

		memcpy(dst16L, src16L, size * 2);
		if(sound_channels == 2)
			memcpy(dst16R, src16R, size * 2);

		lastbuf -= size;
		lastpos += size;
		buffer_end += size;
		buffer_end %= buffer_size;
		buffer_samples += size;

		REG_IME = oldval;
	}
}

int madplay(struct media *m) {
	struct stat s;

	readbufL = (s16 *) malloc(MAD_DECODED_BUFFER_SIZE * 2);
	readbufR = (s16 *) malloc(MAD_DECODED_BUFFER_SIZE * 2);
	buffer = (char *) malloc(FILE_BUFFER_SIZE);

	InputBuffer = (char *) malloc(INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD);
	stat(m->path, &s);
	mad_size = s.st_size;

	sound_playfile = fopen(m->path, "r");

	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);
//	mad_timer_reset(&Timer);

	buflen = 0;
	lastbuf = lastpos = 0;
//	FrameCount = 0;

	madplay_update();
	sound_bps = 2;

	format = MAD;

	return 0;
}

void madplay_flush(void) {
	mad_synth_finish(&Synth);
	mad_frame_finish(&Frame);
	mad_stream_finish(&Stream);

	mad_stream_init(&Stream);
	mad_frame_init(&Frame);
	mad_synth_init(&Synth);

	buflen = 0;
	lastbuf = lastpos = 0;
}

u32 madplay_position(void) {
	return ftell(sound_playfile) - buflen;
}

u32 madplay_size(void) {
	return mad_size;
}

void madplay_seek(u32 pos) {
	fseek(sound_playfile, pos, SEEK_SET);
}
