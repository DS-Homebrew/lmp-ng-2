#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>

#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <ipc2.h>

#define SOUND_NEED_BUFFER

#include "sound.h"
#include "file.h"

#define MIN(a,b) ((a > b) ? (b) : (a))

static OggVorbis_File vf;
static int current_section;
static char *readbuffer;

void tremor_update(void) {
	int ret;

	sound_elapsed = ov_pcm_tell(&vf) / sound_samplerate;
	sound_time = ov_pcm_total(&vf, -1) / sound_samplerate;

	if(state == FINISHING)
		return;

	if(sound_channels == 1) {
		while(buffer_samples < buffer_size) {
			ret = ov_read(&vf, (void *) (pcmbufL + buffer_end * 2), 2 * MIN(buffer_size - buffer_samples, buffer_size - buffer_end), &current_section);

			if(ret <= 0) {
				state = FINISHING;
				return;
			}

			buffer_end += ret / 2;
			buffer_end %= buffer_size;
			buffer_samples += ret / 2;
		}

	} else if(sound_channels == 2) {
		int i;
		s16 *srcL, *dstL, *srcR, *dstR;
		int oldval;

		while(buffer_samples < buffer_size) {
			ret = ov_read(&vf, readbuffer, 4 * MIN(buffer_size - buffer_samples, buffer_size - buffer_end), &current_section);

			if(ret <= 0) {
				state = FINISHING;
				return;
			}

			srcL = (s16 *) readbuffer;
			dstL = ((s16 *) pcmbufL) + buffer_end;
			srcR = ((s16 *) readbuffer) + 1;
			dstR = ((s16 *) pcmbufR) + buffer_end;

			/* we can't be interrupted here or the sound will be corrupted! */
			oldval = REG_IME;
			REG_IME = 0;

			for(i = 0; i < ret / 2; i += 2) {
				dstL[i/2] = srcL[i];
				dstR[i/2] = srcR[i];
			}

			buffer_end += ret / 4;
			buffer_end %= buffer_size;
			buffer_samples += ret / 4;

			REG_IME = oldval;
		}
	}
}

int tremor_play(struct media *m) {
	long len;
	int ret;
	FILE *fp;
	vorbis_info *vi;

	readbuffer = (char *) malloc(buffer_size * 4);

	fp = fopen(m->path, "r");

	format = TREMOR;

	ov_open(fp, &vf, NULL, 0);

	current_section = 0;

	len = ov_pcm_total(&vf,-1);
	vi = ov_info(&vf,-1);

	sound_channels = vi->channels;
	sound_samplerate = vi->rate;
	sound_bps = 2;

	tremor_update();

	return 0;
}

void tremor_stop(void) {
	ov_clear(&vf);
	free(readbuffer);
}

void tremor_flush(void) {
}

u32 tremor_size(void) {
	return ov_pcm_total(&vf, -1);
}

u32 tremor_position(void) {
	return ov_pcm_tell(&vf);
}

void tremor_seek(u32 pos) {
	ov_pcm_seek(&vf, pos);
}
