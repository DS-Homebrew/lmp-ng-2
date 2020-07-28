#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define PLAYLIST_C

#include "playlist.h"
#include "file.h"

#define BLOCK 1

struct playlists playlists;
struct playlist *shufflelist;
int shufflepos;

struct playlist *playlist_build_songs(void) {
	struct media *m;
	struct playlist *p;

	p = (struct playlist *) malloc(sizeof(struct playlist));
	p->size = p->current = 0;

	m = media;
	do {
		p->size++;

		m = m->next;
	} while(m != media);

	p->list = (struct media **) malloc(p->size * sizeof(struct media *));
	p->size = 0;

	m = media;
	do {
		p->list[p->size++] = m;

		m = m->next;
	} while(m != media);

//	p->name = (char *) malloc(6 * sizeof(char));
//	strcpy(p->name, "Songs");
	p->name = "Songs";

	return p;
}

int rand_n(int n) {
	int r;

	if(n == 0 || n == 1)
		return 0;

	do {
		r = rand();
	} while(r > RAND_MAX - RAND_MAX % n);

	return r % n;
}

void playlist_shuffle(struct playlist *p) {
	int i, r;
	struct media *t;

	if(shufflelist != NULL) {
		free(shufflelist->list);
		free(shufflelist);
		shufflelist = NULL;
	}

	shufflelist = (struct playlist *) malloc(sizeof(struct playlist));
	shufflelist->current = 0;
	shufflelist->size = p->size;

	shufflelist->list = (struct media **) malloc(shufflelist->size * sizeof(struct media *));

	for(i = 0; i < p->size; i++) {
		shufflelist->list[i] = p->list[i];
	}

	for(i = 0; i < p->size; i++) {
		r = rand_n(i+1);

		t = shufflelist->list[i];
		shufflelist->list[i] = shufflelist->list[r];
		shufflelist->list[r] = t;
	}

	shufflelist->name = "Shuffle Songs";
}

int playlist_add(struct playlist *p) {
	static int block = BLOCK;

	if(playlists.size == 0)
		playlists.p = (struct playlist **) malloc(block * sizeof(struct playlist *));
	else if(playlists.size == block) {
		block <<= 1;

		playlists.p = (struct playlist **) realloc(playlists.p, block * sizeof(struct playlist *));
	}

	playlists.p[playlists.size++] = p;

	return playlists.size - 1;
}

int playlist_change(int pos, struct playlist *p) {
	playlists.p[pos] = p;
}

struct playlist *playlist_process(char *path, char *filename) {
	char buffer[512], buffer2[1024];
	char file[1024];
	int len;
	FILE *fp;
	struct playlist *p;
	struct media *m;

	file[0] = '\0';
	if(file[strlen(path) - 1] != '/')
		strcat(path, "/");
	strcat(file, path);
	strcat(file, filename);

	p = (struct playlist *) malloc(sizeof(struct playlist));
	p->size = p->current = 0;
	p->name = (char *) malloc(strlen(filename) + 1);
	strcpy(p->name, filename);

	fp = fopen(file, "r");

	while(fgets(buffer, 512, fp) != NULL)
		p->size++;

	p->list = (struct media **) malloc(p->size * sizeof(struct media *));

	p->size = 0;

	fseek(fp, 0, SEEK_SET);

	while(fgets(buffer, 512, fp) != NULL) {
		len = strlen(buffer);

		if(buffer[len - 1] == '\n')
			buffer[--len] = '\0';
		if(buffer[len - 1] == '\r')
			buffer[--len] = '\0';

		buffer2[0] = '\0';
		strcat(buffer2, path);
		strcat(buffer2, buffer);

		m = media;
		do {
			if(strcasecmp(buffer2, m->path) == 0)
				break;

			m = m->next;
		} while(m != media);

		if(strcasecmp(m->path, buffer2) == 0)
			p->list[p->size++] = m;
	}

	fclose(fp);

	return p;
}
