#ifndef PLAYLIST_H
#define PLAYLIST_H

#define PLAYLIST (playlists.p[playlists.current])

struct playlist {
	u32 size;
	u32 current;

	char *name;

	struct media **list;
};

struct playlists {
	u32 size;
	u32 current;

	struct playlist **p;
};

#ifndef PLAYLIST_C
extern struct playlists playlists;
extern struct playlist *shufflelist;
extern int shufflepos;
#endif

struct playlist *playlist_build_songs(void);
int playlist_add(struct playlist *p);
struct playlist *playlist_process(char *path, char *filename);

#endif
