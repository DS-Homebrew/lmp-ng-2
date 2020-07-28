#ifndef SCREEN_H
#define SCREEN_H

#include "playlist.h"

#define SCREEN_MAX_SCROLLING_TEXT 16

struct screen_scrolling_text {
	u16 *data;
	u16 width, height;
	u16 maxwidth;
	u16 x, y;
	u16 x_offset;
	u32 delay;
	u8 animate;

	u16 *background;
	u16 bg_width;
};

struct screen_menu_entry {
	char *label;
	u8 have_submenu;
	void (*callback)(int, const char*);
};

#ifndef SCREEN_C
extern
#endif
enum screen_type { MENU, NOW_PLAYING, OTHER } screen_type;

#ifndef SCREEN_C
extern
#endif
enum screen_menu { MAIN, MUSIC, LIST, LISTS, SETTINGS, SKINS } screen_menu;

#ifndef SCREEN_C
extern
#endif
struct screen_menu_entry *currentmenu;

#ifndef SCREEN_C
extern
#endif
u32 screen_menu_selected;

#ifndef SCREEN_C
extern
#endif
u32 screen_menu_startentry;

#ifndef SCREEN_C
extern
#endif
u32 screen_menu_items;

#ifndef SCREEN_C
extern
#endif
struct screen_scrolling_text screen_scrolling_text[SCREEN_MAX_SCROLLING_TEXT];

void screen_update(void);
void screen_mainmenu(void);
void screen_nowplaying(void);
void screen_nowplaying_reset(void);
void screen_playlistmenu(struct playlist *);

#endif
