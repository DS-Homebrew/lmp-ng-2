#include <nds.h>

#include <FLAC/stream_decoder.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#define SOUND_NEED_BUFFER

#include "sound.h"
#include "file.h"

#define FLAC_BUFFER_SIZE (65536) /* maximum blocksize according to flac specs */

#define MIN(A,B) ((A > B) ? (B) : (A))

FLAC__StreamDecoder *d;
static char *readbufL, *readbufR;
static int lastbuf, lastpos;
static u32 position;

FLAC__StreamDecoderWriteStatus flac_write(const FLAC__StreamDecoder *d, const FLAC__Frame *frame, const FLAC__int32 *const buffer[], void *client_data) {
	int i, j;
	s16 *dst[2];

	if(sound_channels == 0) {
		sound_channels = frame->header.channels;
		sound_samplerate = frame->header.sample_rate;
		sound_bps = frame->header.bits_per_sample / 8;
	}

	dst[0] = ((s16 *) readbufL);
	dst[1] = ((s16 *) readbufR);
	for(i=0; i < frame->header.channels; i++) {
		for(j = 0; j < frame->header.blocksize; j++)
			dst[i][j] = (s16) buffer[i][j];
	}

	lastbuf = frame->header.blocksize;

	return FLAC__STREAM_DECODER_WRITE_STATUS_CONTINUE;
}

void flac_metadata(const FLAC__StreamDecoder *decoder, const FLAC__StreamMetadata *metadata, void *client_data) {
}

void flac_error(const FLAC__StreamDecoder *decoder, FLAC__StreamDecoderErrorStatus status, void *client_data) {
	state = FINISHING;
}

void flac_update(void) {
	int oldval;
	int size;

	void *srcL, *srcR, *dstL, *dstR;

	sound_time = FLAC__stream_decoder_get_total_samples(d) / sound_samplerate;
	sound_elapsed = position / sound_samplerate;

	while(buffer_samples < buffer_size) {
		if(lastbuf == 0) {
			FLAC__StreamDecoderState s;

			while(lastbuf == 0) {
				s = FLAC__stream_decoder_get_state(d);

				if(s == FLAC__STREAM_DECODER_END_OF_STREAM) {
					state = FINISHING;
					return;
				}

				if(FLAC__stream_decoder_process_single(d) != 1) {
					state = ERROR;
					return;
				}
			}
			lastpos = 0;
		}

		size = MIN(buffer_size - buffer_end, buffer_size - buffer_samples);
		size = MIN(size, lastbuf);

		if(sound_bps == 2) {
			srcL = (void *) (((s16 *) readbufL) + lastpos);
			dstL = (void *) (((s16 *) pcmbufL) + buffer_end);
			srcR = (void *) (((s16 *) readbufR) + lastpos);
			dstR = (void *) (((s16 *) pcmbufR) + buffer_end);
		} else if(sound_bps == 1) {
			srcL = (void *) (((s8 *) readbufL) + lastpos);
			dstL = (void *) (((s8 *) pcmbufL) + buffer_end);
			srcR = (void *) (((s8 *) readbufR) + lastpos);
			dstR = (void *) (((s8 *) pcmbufR) + buffer_end);
		}

		/* we can't be interrupted here or the sound will be corrupt
		 * this shold be very fast, we're only copying the buffers */
		oldval = REG_IME;
		REG_IME = 0;

		memcpy(dstL, srcL, size * sound_bps);
		if(sound_channels == 2)
			memcpy(dstR, srcR, size * sound_bps);

		lastbuf -= size;
		lastpos += size;
		buffer_end += size;
		buffer_end %= buffer_size;
		buffer_samples += size;
		position += size;

		REG_IME = oldval;
	}
}

void flac_stop(void) {
	if(d != NULL)
		FLAC__stream_decoder_finish(d);

	free(readbufL);
	free(readbufR);
}

int flac_play(struct media *m) {
	format = UNKNOWN;

	readbufL = (char *) malloc(FLAC_BUFFER_SIZE * 2);
	readbufR = (char *) malloc(FLAC_BUFFER_SIZE * 2);
	lastpos = lastbuf = 0;
	position = 0;
	FLAC__StreamDecoderInitStatus initstatus;

	d = FLAC__stream_decoder_new();

	if(d == NULL) {
		state = ERROR;
		flac_stop();
		return 1;
	}

	initstatus = FLAC__stream_decoder_init_file(d, m->path, flac_write, flac_metadata, flac_error, NULL);

	if(initstatus != FLAC__STREAM_DECODER_INIT_STATUS_OK) {
		state = ERROR;
		flac_stop();
		return 1;
	}

	FLAC__stream_decoder_process_until_end_of_metadata(d);

	sound_channels = 0;

	flac_update();

	if(state == FINISHING || state == ERROR) {
		flac_stop();
		state = ERROR;
		return 1;
	}

	format = FLAC;

	return 0;
}

void flac_flush(void) {
	lastbuf = lastpos = 0;
}

u32 flac_size(void) {
	return FLAC__stream_decoder_get_total_samples(d);
}

u32 flac_position(void) {
	return position;
}

void flac_seek(u32 pos) {
	FLAC__stream_decoder_seek_absolute(d, pos);
	position = pos;
}
