#ifndef FILE_H
#define FILE_H

enum format { WAVE, MAD, TREMOR, FLAC, UNKNOWN };

struct media {
	char *title;
	char *artist;
	char *album;

	char *path;

	enum format format;

	struct media *prev, *next;
};

#ifndef FILE_C
extern struct media *media;
extern struct media *currentmedia;
extern u8 file_scan_id3;
#endif

struct media *media_alloc(void);
int file_scan(char *path);
int file_scan_playlists(char *path);
void file_sort(void);

#endif
