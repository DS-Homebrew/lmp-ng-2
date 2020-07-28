#ifndef INPUT_H
#define INPUT_H

#include "wheel.h"

#define INPUT_HOLD_OFF 1
#define INPUT_HOLD_NORMAL 2
#define INPUT_HOLD_ALL 3

enum input_type { WHEEL, KEY_PRESS, KEY_HELD, END };

struct input_handle {
	enum input_type type;
	u32 key;
	void (*callback)(void);
};

void input_handleinput(void);
void input_nowplaying_setup(int, const char *);
void input_musicmenu_setup(int, const char *);
void input_playlist(int, const char *);
void input_playlists(int, const char *);
void input_songs_setup(int, const char *);
void input_playlist_setup(int, const char *);
void input_playlists_setup(int, const char *);
void input_sound_finished();
void input_shuffle_setup(int pos, const char *text);
void input_tags_scan(int pos, const char *text);
void input_settings_setup(int pos, const char *text);
void input_skins_setup(int pos, const char *text);

#ifdef INPUT_C
static void menu_down(void);
static void menu_up(void);
static void menu_center(void);
static void menu_play(void);
static void menu_next(void);
static void menu_prev(void);
static void menu_stop(void);

static void nowplaying_back(void);
static void nowplaying_next(void);
static void nowplaying_prev(void);
static void nowplaying_playpause(void);
static void nowplaying_right(void);
static void nowplaying_left(void);
static void nowplaying_R(void);
static void nowplaying_L(void);
static void nowplaying_changemode(void);

static void music_back(void);
static void music_up(void);
static void music_down(void);
static void music_center(void);

static void playlist_back(void);
static void playlist_up(void);
static void playlist_down(void);
static void playlist_center(void);

static void playlists_back(void);
static void playlists_up(void);
static void playlists_down(void);
static void playlists_center(void);

static void settings_back(void);
static void settings_up(void);
static void settings_down(void);
static void settings_center(void);

static void skins_back(void);
static void skins_up(void);
static void skins_down(void);
static void skins_center(void);
#endif


#ifndef INPUT_C
extern
#endif
u8 input_hold;

#ifndef INPUT_C
extern
#endif
struct input_handle *currenthandle;

#ifndef INPUT_C
extern
#endif
enum nowplaying_mode { NOWPLAYING_VOLUME, NOWPLAYING_VOLUME_CHANGING, NOWPLAYING_SEEK } nowplaying_mode;

#ifndef INPUT_C
extern
#endif
int nowplaying_delay;

#ifdef INPUT_C
static struct input_handle menu_input[] = {
	{ WHEEL, WHEEL_RIGHT, menu_down },
	{ KEY_PRESS, KEY_DOWN, menu_down },
	{ KEY_HELD, KEY_R, menu_down },
	{ WHEEL, WHEEL_LEFT, menu_up },
	{ KEY_PRESS, KEY_UP, menu_up },
	{ KEY_HELD, KEY_L, menu_up },
	{ WHEEL, WHEEL_CENTER, menu_center },
	{ KEY_PRESS, KEY_RIGHT, menu_center },
//	{ KEY_PRESS, KEY_B, menu_play },
//	{ KEY_PRESS, KEY_A, menu_next },
//	{ KEY_PRESS, KEY_Y, menu_prev },
//	{ KEY_PRESS, KEY_X, menu_stop },
	{ END, 0, NULL }
};
struct input_handle *menu_input_p = menu_input;
#else
extern struct input_handle *menu_input_p;
#endif

#ifdef INPUT_C
static struct input_handle nowplaying_input[] = {
	{ WHEEL, WHEEL_MENU, nowplaying_back },
	{ KEY_PRESS, KEY_LEFT, nowplaying_back },
	{ KEY_PRESS, KEY_X, nowplaying_back },
	{ WHEEL, WHEEL_FORWARD, nowplaying_next },
	{ KEY_PRESS, KEY_A, nowplaying_next },
	{ WHEEL, WHEEL_REWIND, nowplaying_prev },
	{ KEY_PRESS, KEY_Y, nowplaying_prev },
	{ WHEEL, WHEEL_PLAY, nowplaying_playpause },
	{ KEY_PRESS, KEY_B, nowplaying_playpause },
	{ WHEEL, WHEEL_RIGHT, nowplaying_right },
	{ WHEEL, WHEEL_LEFT, nowplaying_left },
	{ KEY_HELD, KEY_R, nowplaying_right },
	{ KEY_HELD, KEY_L, nowplaying_left },
	{ KEY_PRESS, KEY_DOWN, nowplaying_right },
	{ KEY_PRESS, KEY_UP, nowplaying_left },
	{ KEY_PRESS, KEY_R, nowplaying_R },
	{ KEY_PRESS, KEY_L, nowplaying_L },
	{ WHEEL, WHEEL_CENTER, nowplaying_changemode },
	{ KEY_PRESS, KEY_RIGHT, nowplaying_changemode },
	{ END, 0, NULL }
};
struct input_handle *nowplaying_input_p = nowplaying_input;
#else
extern struct input_handle *nowplaying_input_p;
#endif

#ifdef INPUT_C
static struct input_handle music_input[] = {
	{ WHEEL, WHEEL_MENU, music_back },
	{ KEY_PRESS, KEY_LEFT, music_back },
	{ KEY_PRESS, KEY_X, music_back },
	{ WHEEL, WHEEL_RIGHT, music_down },
	{ KEY_PRESS, KEY_DOWN, music_down },
	{ KEY_HELD, KEY_R, music_down },
	{ WHEEL, WHEEL_LEFT, music_up },
	{ KEY_PRESS, KEY_UP, music_up},
	{ KEY_HELD, KEY_L, music_up},
	{ WHEEL, WHEEL_CENTER, music_center },
	{ KEY_PRESS, KEY_RIGHT, music_center },
	{ END, 0, NULL }
};
struct input_handle *music_input_p = music_input;
#else
extern struct input_handle *music_input_p;
#endif

#ifdef INPUT_C
static struct input_handle playlist_input[] = {
	{ WHEEL, WHEEL_MENU, playlist_back },
	{ KEY_PRESS, KEY_LEFT, playlist_back },
	{ KEY_PRESS, KEY_X, playlist_back },
	{ WHEEL, WHEEL_RIGHT, playlist_down },
	{ KEY_PRESS, KEY_DOWN, playlist_down },
	{ KEY_HELD, KEY_R, playlist_down },
	{ WHEEL, WHEEL_LEFT, playlist_up },
	{ KEY_PRESS, KEY_UP, playlist_up },
	{ KEY_HELD, KEY_L, playlist_up },
	{ WHEEL, WHEEL_CENTER, playlist_center },
	{ KEY_PRESS, KEY_RIGHT, playlist_center },
	{ END, 0, NULL }
};
struct input_handle *playlist_input_p = playlist_input;
#else
extern struct input_handle *playlist_input_p;
#endif

#ifdef INPUT_C
static struct input_handle playlists_input[] = {
	{ WHEEL, WHEEL_MENU, playlists_back },
	{ KEY_PRESS, KEY_LEFT, playlists_back },
	{ KEY_PRESS, KEY_X, playlists_back },
	{ WHEEL, WHEEL_RIGHT, playlists_down },
	{ KEY_PRESS, KEY_DOWN, playlists_down },
	{ KEY_HELD, KEY_R, playlists_down },
	{ WHEEL, WHEEL_LEFT, playlists_up },
	{ KEY_PRESS, KEY_UP, playlists_up},
	{ KEY_HELD, KEY_L, playlists_up },
	{ WHEEL, WHEEL_CENTER, playlists_center },
	{ KEY_PRESS, KEY_RIGHT, playlists_center },
	{ END, 0, NULL }
};
struct input_handle *playlists_input_p = playlists_input;
#else
extern struct input_handle *playlists_input_p;
#endif

#ifdef INPUT_C
static struct input_handle settings_input[] = {
	{ WHEEL, WHEEL_MENU, settings_back },
	{ KEY_PRESS, KEY_LEFT, settings_back },
	{ KEY_PRESS, KEY_X, settings_back },
	{ WHEEL, WHEEL_RIGHT, settings_down },
	{ KEY_PRESS, KEY_DOWN, settings_down },
	{ KEY_HELD, KEY_R, settings_down },
	{ WHEEL, WHEEL_LEFT, settings_up },
	{ KEY_PRESS, KEY_UP, settings_up},
	{ KEY_HELD, KEY_L, settings_up},
	{ WHEEL, WHEEL_CENTER, settings_center },
	{ KEY_PRESS, KEY_RIGHT, settings_center },
	{ END, 0, NULL }
};
struct input_handle *settings_input_p = settings_input;
#else
extern struct input_handle *settings_input_p;
#endif

#ifdef INPUT_C
static struct input_handle skins_input[] = {
	{ WHEEL, WHEEL_MENU, skins_back },
	{ KEY_PRESS, KEY_LEFT, skins_back },
	{ KEY_PRESS, KEY_X, skins_back },
	{ WHEEL, WHEEL_RIGHT, skins_down },
	{ KEY_PRESS, KEY_DOWN, skins_down },
	{ KEY_HELD, KEY_R, skins_down },
	{ WHEEL, WHEEL_LEFT, skins_up },
	{ KEY_PRESS, KEY_UP, skins_up},
	{ KEY_HELD, KEY_L, skins_up},
	{ WHEEL, WHEEL_CENTER, skins_center },
	{ KEY_PRESS, KEY_RIGHT, skins_center },
	{ END, 0, NULL }
};
struct input_handle *skins_input_p = skins_input;
#else
extern struct input_handle *skins_input_p;
#endif

#endif
