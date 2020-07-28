struct progressbar {
	int type;

	u16 xpos, ypos, xsize, ysize;

	u16 *seekpos;
	u16 seekpos_size, seekpos_ypos;

	u16 *seekbar;
	u16 seekbar_size;

	u16 *full;
	u16 full_size;
	u16 full_animate;
	u16 *empty;
	u16 empty_size;
};

struct skin {
	char *file;

	u16 width, height;
	u16 xpos, ypos;

	char *main_bg, *sub_bg;

	char *battery_icons;
	u16 battery_xpos, battery_ypos, battery_size;

	char *state_icons;
	u16 state_xpos, state_ypos, state_size;

	char *hold_icons;
	u16 hold_xpos, hold_ypos, hold_size;

	char *shuffle_icons;
	u16 shuffle_xpos, shuffle_ypos, shuffle_size;

	char *volume_icons;
	u16 volume_xpos, volume_ypos, volume_size;

	char *unknownicon;
	char *selected;

	char *seekbar_sprite, *seekbar_bar, *seekbar_full, *seekbar_empty;
	struct progressbar seekbar;

	char *progressbar_sprite, *progressbar_bar, *progressbar_full, *progressbar_empty;
	struct progressbar progressbar;

	char *volumebar_sprite, *volumebar_bar, *volumebar_full, *volumebar_empty;
	struct progressbar volumebar;

	u16 position_xpos, position_ypos, position_centered;

	u16 album_xpos, album_ypos, album_centered;

	u16 title_xpos, title_ypos, title_centered, title_maxsize;

	u16 mediatitle_xpos, mediatitle_ypos, mediatitle_centered, mediatitle_maxsize;

	u16 mediaartist_xpos, mediaartist_ypos, mediaartist_centered, mediaartist_maxsize;

	u16 mediaalbum_xpos, mediaalbum_ypos, mediaalbum_centered, mediaalbum_maxsize;
	
	u16 title_mask, menu_mask, selected_mask, position_mask, mediatitle_mask, mediaartist_mask, mediaalbum_mask;

	u16 time_xpos, time_ypos, time_mask;
};

#ifndef SKIN_C
extern
#endif
struct skin currentskin;

void skin_init(char *path);
u16 *skin_get_main_bg(void);
u16 *skin_get_sub_bg(void);
u16 **skin_get_font(void);
u16 skin_get_width(void);
u16 skin_get_height(void);
u16 skin_get_xpos(void);
u16 skin_get_ypos(void);
void skin_get_batteryicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size);
void skin_get_stateicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size);
void skin_get_holdicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size);
u16 * skin_get_unknownicon(void);
void skin_get_shuffleicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size);
void skin_get_volumeicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size);
u16 *skin_get_selected(void);
void skin_get_seekbar(struct progressbar **p);
void skin_get_progressbar(struct progressbar **p);
void skin_get_volumebar(struct progressbar **p);
void skin_get_positionpos(u16 *xpos, u16 *ypos, u16 *centered);
void skin_get_albumpos(u16 *xpos, u16 *ypos, u16 *centered);
void skin_get_titlepos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize);
void skin_get_mtitlepos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize);
void skin_get_martistpos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize);
void skin_get_malbumpos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize);
