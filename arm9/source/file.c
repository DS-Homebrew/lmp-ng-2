#include <fat.h>

#include <nds.h>

#include <sys/dir.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_C
#include "file.h"

#include "heap.h"

#define BLOCK 1

static struct media *freelist;
struct media *media;
struct media *currentmedia;

static void wait(void) {
	int i;

	for(i=0; i<60; i++)
		swiWaitForVBlank();
}

struct media *media_alloc(void) {
	static int block = BLOCK;
	struct media *r;

	if(freelist == NULL) {
		int i;

		freelist = (struct media *) malloc(block * sizeof(struct media));
		if(freelist == NULL)
			return NULL;

		for(i=0; i<block; i++)
			freelist[i].next  = &freelist[i+1];
		freelist[block-1].next = NULL;

		block <<= 1;
	}

	r = freelist;
	freelist = r->next;

	return r;
}

static enum format verify_type(char *path, char *filename) {
	int s = strlen(filename);

	if(s < 3)
		return UNKNOWN;

	if(tolower(filename[s-1]) == '3' && tolower(filename[s-2]) == 'p' && tolower(filename[s-3]) == 'm')
		return MAD;

	if(tolower(filename[s-1]) == 'g' && tolower(filename[s-2]) == 'g' && tolower(filename[s-3]) == 'o')
		return TREMOR;

	if(tolower(filename[s-1]) == 'c' && tolower(filename[s-2]) == 'a' && tolower(filename[s-3]) == 'l')
		return FLAC;

	return UNKNOWN;
}

static void add_media(char *path, char *filename, enum format type) {
	struct media *m;
	int plen = strlen(path);

	m = media_alloc();

	if(m == NULL) {
		printf("add_media: malloc failed!\n");
		while(1)
			swiWaitForVBlank();
	}

	m->path = (char *) malloc(plen + strlen(filename) + 2);

	if(m->path == NULL) {
		printf("add_media: malloc failed!\n");
		while(1)
			swiWaitForVBlank();
	}

	strcpy(m->path, path);
	if(path[plen - 1] != '/')
		strcat(m->path, "/");
	strcat(m->path, filename);
	m->format = type;

	m->artist = NULL;
	m->album = NULL;

	m->title = strdup(filename);

	if(m->title == NULL) {
		printf("add_media: malloc failed!\n");
		while(1)
			swiWaitForVBlank();
	}

	if(media == NULL) {
		media = m;
		m->next = m;
		m->prev = m;
	} else {
		m->next = media;
		m->prev = media->prev;
		media->prev->next = m;
		media->prev = m;
		media = m;
	}
}

static int heap_cmp(const void *e1, const void *e2) {
	const struct media *m1 = (const struct media *) e1,
				*m2 = (const struct media *) e2;

	if(e1 == e2)
		return 0;

	if(e1 == NULL)
		return -1;
	if(e2 == NULL)
		return 1;

	return strcasecmp(m1->path, m2->path);
}

int file_scan(char *path) {
	DIR_ITER *d;
	struct stat s;
	char filename[256];
	char *queue[1024];
	unsigned qinit, qend;

	int i;

	qinit = qend = 0;
	queue[qend] = strdup(path);;

	if(queue[qend] == NULL) {
		printf("file_scan: malloc failed!\n");
		while(1)
			swiWaitForVBlank();
	}

	qend++;

	while(qinit < qend) {

		d = diropen(queue[qinit]);
		if(d == NULL) {
			return 1;
		}

		while(dirnext(d, filename, &s) != -1) {
			if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
				continue;
			// HACK
			if(strcasecmp(filename, "moonshl") == 0)
				continue;

			if(S_ISDIR(s.st_mode)) {
				int plen = strlen(queue[qinit]);

				queue[qend] = (char *) malloc(plen + strlen(filename) + 2);
				if(queue[qend] == NULL) {
					printf("file_scan: malloc failed!\n");
					while(1)
						swiWaitForVBlank();
				}
				strcpy(queue[qend], queue[qinit]);
				if(queue[qinit][plen-1] != '/')
					strcat(queue[qend], "/");
				strcat(queue[qend], filename);
				qend++;
			} else if(S_ISREG(s.st_mode)) {
				enum format type;

				if((type = verify_type(queue[qinit], filename)) != UNKNOWN) {
					add_media(queue[qinit], filename, type);
				}
			}
		}

		if(dirclose(d) == -1)
			return 1;

		free(queue[qinit]);
		qinit++;
	}

	return 0;
}

static int verify_playlist(char *path, char *filename) {
	int s = strlen(filename);

	if(s < 3)
		return 0;

	if(tolower(filename[s-1]) == 'u' && tolower(filename[s-2]) == '3' && tolower(filename[s-3]) == 'm')
		return 1;

	return 0;
}

int file_scan_playlists(char *path) {
	DIR_ITER *d;
	struct stat s;
	char filename[256];
	char *queue[1024], qinit, qend;
	int i;

	qinit = qend = 0;
	queue[qend] = strdup(path);

	if(queue[qend] == NULL) {
		printf("file_scan_playlists: malloc failed!\n");
		while(1)
			swiWaitForVBlank();
	}

	qend++;

	while(qinit < qend) {
		d = diropen(queue[qinit]);

		if(d == NULL)
			return 1;

		while(dirnext(d, filename, &s) != -1) {

			if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
				continue;

			if(S_ISDIR(s.st_mode)) {
				int plen = strlen(queue[qinit]);

				queue[qend] = (char *) malloc(plen + strlen(filename) + 2);
				if(queue[qend] == NULL) {
					printf("file_scan_playlists: malloc failed!\n");
					while(1)
						swiWaitForVBlank();
				}

				strcpy(queue[qend], queue[qinit]);
				if(queue[qinit][plen-1] != '/')
					strcat(queue[qend], "/");
				strcat(queue[qend], filename);
				qend++;
			} else if(S_ISREG(s.st_mode)) {
				if((verify_playlist(queue[qinit], filename)) == 1) {
					playlist_add(playlist_process(queue[qinit], filename));
				}
			}
		}

		if(dirclose(d) == -1)
			return 1;

		free(queue[qinit]);
		qinit++;
	}

	return 0;
}

void file_sort(void) {
	tHEAP h;
	struct media *m;

	if(media == NULL)
		return;

	h = heap_init(BLOCK, heap_cmp);

	while(media->next != media) {
		max_heap_insert(h, media);

		media->prev->next = media->next;
		media->next->prev = media->prev;
		media = media->next;
	}
	max_heap_insert(h, media);

	m = heap_extract_max(h);
	m->prev = m;
	m->next = m;
	media = m;

	while((m = heap_extract_max(h)) != NULL) {
		m->next = media->next;
		m->prev = media;
		media->next->prev = m;
		media->next = m;
	}
	media = media->next;

	heap_destroy(h);
}
