#include <nds.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "unzip.h"

#define SKIN_C
#include "skin.h"

#include "myriad_lmp.h"

#define RGBR(x) (x >> 10 & 31)
#define RGBG(x) (x >> 5 & 31)
#define RGBB(x) (x & 31)
#define RGBTOBGR(x) (RGB15(RGBR(x), RGBG(x), RGBB(x)) | (x & (1 << 15)))

struct bmheader {
	unsigned short type;
	unsigned size;
	unsigned short reserved1, reserved2;
	unsigned offset;
} __attribute__((__packed__));

struct bminfoheader {
	unsigned size;
	int w, h;
	unsigned short planes;
	unsigned short bits;
	unsigned compression;
	unsigned imagesize; 
	int xres, yres;
	unsigned ncolors;
	unsigned impcolors;
} __attribute__((__packed__));

static void skin_parse(char *file, unsigned size) {
	int pos;
	int i;

	int start, end;

	char name[128], value[128], *write;
	int t;

	pos = 0;

	do {
		for(i = pos; i < size && file[i] != '\n'; i++);
		start = pos;
		end = i;
		pos = i + 1;

		name[0] = value[0] = '\0';
		write = name;
		t = 0;
		for(i = start; i < end; i++) {
			if(!isspace(file[i]))
				write[t++] = file[i];

			if(file[i] == '=') {
				write[t-1] = '\0';
				t = 0;
				write = value;
			}
		}
		write[t] = '\0';

		if(name[0] == '\0' || value[0] == '\0')
			continue;


		if(strcmp(name, "width") == 0) {
			currentskin.width = atoi(value);
		} else if(strcmp(name, "height") == 0) {
			currentskin.height = atoi(value);
		} else if(strcmp(name, "xpos") == 0) {
			currentskin.xpos = atoi(value);
		} else if(strcmp(name, "ypos") == 0) {
			currentskin.ypos = atoi(value);
		} else if(strcmp(name, "main_bg") == 0) {
			currentskin.main_bg = strdup(value);
		} else if(strcmp(name, "sub_bg") == 0) {
			currentskin.sub_bg = strdup(value);
		} else if(strcmp(name, "battery_icons") == 0) {
			currentskin.battery_icons = strdup(value);
		} else if(strcmp(name, "battery_xpos") == 0) {
			currentskin.battery_xpos = atoi(value);
		} else if(strcmp(name, "battery_ypos") == 0) {
			currentskin.battery_ypos = atoi(value);
		} else if(strcmp(name, "battery_size") == 0) {
			currentskin.battery_size = atoi(value);
		} else if(strcmp(name, "state_icons") == 0) {
			currentskin.state_icons = strdup(value);
		} else if(strcmp(name, "state_xpos") == 0) {
			currentskin.state_xpos = atoi(value);
		} else if(strcmp(name, "state_ypos") == 0) {
			currentskin.state_ypos = atoi(value);
		} else if(strcmp(name, "state_size") == 0) {
			currentskin.state_size = atoi(value);
		} else if(strcmp(name, "hold_icons") == 0) {
			currentskin.hold_icons = strdup(value);
		} else if(strcmp(name, "hold_xpos") == 0) {
			currentskin.hold_xpos = atoi(value);
		} else if(strcmp(name, "hold_ypos") == 0) {
			currentskin.hold_ypos = atoi(value);
		} else if(strcmp(name, "hold_size") == 0) {
			currentskin.hold_size = atoi(value);
		} else if(strcmp(name, "shuffle_icons") == 0) {
			currentskin.shuffle_icons = strdup(value);
		} else if(strcmp(name, "shuffle_xpos") == 0) {
			currentskin.shuffle_xpos = atoi(value);
		} else if(strcmp(name, "shuffle_ypos") == 0) {
			currentskin.shuffle_ypos = atoi(value);
		} else if(strcmp(name, "shuffle_size") == 0) {
			currentskin.shuffle_size = atoi(value);
		} else if(strcmp(name, "volume_icons") == 0) {
			currentskin.volume_icons = strdup(value);
		} else if(strcmp(name, "volume_xpos") == 0) {
			currentskin.volume_xpos = atoi(value);
		} else if(strcmp(name, "volume_ypos") == 0) {
			currentskin.volume_ypos = atoi(value);
		} else if(strcmp(name, "volume_size") == 0) {
			currentskin.volume_size = atoi(value);
		} else if(strcmp(name, "unknown_icon") == 0) {
			currentskin.unknownicon = strdup(value);
		} else if(strcmp(name, "selected_bmp") == 0) {
			currentskin.selected = strdup(value);
		} else if(strcmp(name, "seekbar_type") == 0) {
			currentskin.seekbar.type = atoi(value);
		} else if(strcmp(name, "seekbar_xpos") == 0) {
			currentskin.seekbar.xpos = atoi(value);
		} else if(strcmp(name, "seekbar_ypos") == 0) {
			currentskin.seekbar.ypos = atoi(value);
		} else if(strcmp(name, "seekbar_xsize") == 0) {
			currentskin.seekbar.xsize = atoi(value);
		} else if(strcmp(name, "seekbar_ysize") == 0) {
			currentskin.seekbar.ysize = atoi(value);
		} else if(strcmp(name, "seekbar_spritepos") == 0) {
			currentskin.seekbar_sprite = strdup(value);
		} else if(strcmp(name, "seekbar_spritesize") == 0) {
			currentskin.seekbar.seekpos_size = atoi(value);
		} else if(strcmp(name, "seekbar_spriteypos") == 0) {
			currentskin.seekbar.seekpos_ypos = atoi(value);
		} else if(strcmp(name, "seekbar_bar") == 0) {
			currentskin.seekbar_bar = strdup(value);
		} else if(strcmp(name, "seekbar_barsize") == 0) {
			currentskin.seekbar.seekbar_size = atoi(value);
		} else if(strcmp(name, "seekbar_full") == 0) {
			currentskin.seekbar_full = strdup(value);
		} else if(strcmp(name, "seekbar_full_size") == 0) {
			currentskin.seekbar.full_size = atoi(value);
		} else if(strcmp(name, "seekbar_full_animate") == 0) {
			currentskin.seekbar.full_animate = atoi(value);
		} else if(strcmp(name, "seekbar_empty") == 0) {
			currentskin.seekbar_empty = strdup(value);
		} else if(strcmp(name, "seekbar_empty_size") == 0) {
			currentskin.seekbar.empty_size = atoi(value);
		} else if(strcmp(name, "progressbar_type") == 0) {
			currentskin.progressbar.type = atoi(value);
		} else if(strcmp(name, "progressbar_xpos") == 0) {
			currentskin.progressbar.xpos = atoi(value);
		} else if(strcmp(name, "progressbar_ypos") == 0) {
			currentskin.progressbar.ypos = atoi(value);
		} else if(strcmp(name, "progressbar_xsize") == 0) {
			currentskin.progressbar.xsize = atoi(value);
		} else if(strcmp(name, "progressbar_ysize") == 0) {
			currentskin.progressbar.ysize = atoi(value);
		} else if(strcmp(name, "progressbar_spritepos") == 0) {
			currentskin.progressbar_sprite = strdup(value);
		} else if(strcmp(name, "progressbar_spritesize") == 0) {
			currentskin.progressbar.seekpos_size = atoi(value);
		} else if(strcmp(name, "progressbar_spriteypos") == 0) {
			currentskin.progressbar.seekpos_ypos = atoi(value);
		} else if(strcmp(name, "progressbar_bar") == 0) {
			currentskin.progressbar_bar = strdup(value);
		} else if(strcmp(name, "progressbar_barsize") == 0) {
			currentskin.progressbar.seekbar_size = atoi(value);
		} else if(strcmp(name, "progressbar_full") == 0) {
			currentskin.progressbar_full = strdup(value);
		} else if(strcmp(name, "progressbar_full_size") == 0) {
			currentskin.progressbar.full_size = atoi(value);
		} else if(strcmp(name, "progressbar_full_animate") == 0) {
			currentskin.progressbar.full_animate = atoi(value);
		} else if(strcmp(name, "progressbar_empty") == 0) {
			currentskin.progressbar_empty = strdup(value);
		} else if(strcmp(name, "progressbar_empty_size") == 0) {
			currentskin.progressbar.empty_size = atoi(value);
		} else if(strcmp(name, "volumebar_type") == 0) {
			currentskin.volumebar.type = atoi(value);
		} else if(strcmp(name, "volumebar_xpos") == 0) {
			currentskin.volumebar.xpos = atoi(value);
		} else if(strcmp(name, "volumebar_ypos") == 0) {
			currentskin.volumebar.ypos = atoi(value);
		} else if(strcmp(name, "volumebar_xsize") == 0) {
			currentskin.volumebar.xsize = atoi(value);
		} else if(strcmp(name, "volumebar_ysize") == 0) {
			currentskin.volumebar.ysize = atoi(value);
		} else if(strcmp(name, "volumebar_spritepos") == 0) {
			currentskin.volumebar_sprite = strdup(value);
		} else if(strcmp(name, "volumebar_spritesize") == 0) {
			currentskin.volumebar.seekpos_size = atoi(value);
		} else if(strcmp(name, "volumebar_spriteypos") == 0) {
			currentskin.volumebar.seekpos_ypos = atoi(value);
		} else if(strcmp(name, "volumebar_bar") == 0) {
			currentskin.volumebar_bar = strdup(value);
		} else if(strcmp(name, "volumebar_barsize") == 0) {
			currentskin.volumebar.seekbar_size = atoi(value);
		} else if(strcmp(name, "volumebar_full") == 0) {
			currentskin.volumebar_full = strdup(value);
		} else if(strcmp(name, "volumebar_full_size") == 0) {
			currentskin.volumebar.full_size = atoi(value);
		} else if(strcmp(name, "volumebar_full_animate") == 0) {
			currentskin.volumebar.full_animate = atoi(value);
		} else if(strcmp(name, "volumebar_empty") == 0) {
			currentskin.volumebar_empty = strdup(value);
		} else if(strcmp(name, "volumebar_empty_size") == 0) {
			currentskin.volumebar.empty_size = atoi(value);
		} else if(strcmp(name, "position_xpos") == 0) {
			currentskin.position_xpos = atoi(value);
		} else if(strcmp(name, "position_ypos") == 0) {
			currentskin.position_ypos = atoi(value);
		} else if(strcmp(name, "position_centered") == 0) {
			currentskin.position_centered = atoi(value);
		} else if(strcmp(name, "album_xpos") == 0) {
			currentskin.album_xpos = atoi(value);
		} else if(strcmp(name, "album_ypos") == 0) {
			currentskin.album_ypos = atoi(value);
		} else if(strcmp(name, "album_centered") == 0) {
			currentskin.album_centered = atoi(value);
		} else if(strcmp(name, "title_xpos") == 0) {
			currentskin.title_xpos = atoi(value);
		} else if(strcmp(name, "title_ypos") == 0) {
			currentskin.title_ypos = atoi(value);
		} else if(strcmp(name, "title_centered") == 0) {
			currentskin.title_centered = atoi(value);
		} else if(strcmp(name, "title_maxsize") == 0) {
			currentskin.title_maxsize = atoi(value);
		} else if(strcmp(name, "mediatitle_xpos") == 0) {
			currentskin.mediatitle_xpos = atoi(value);
		} else if(strcmp(name, "mediatitle_ypos") == 0) {
			currentskin.mediatitle_ypos = atoi(value);
		} else if(strcmp(name, "mediatitle_centered") == 0) {
			currentskin.mediatitle_centered = atoi(value);
		} else if(strcmp(name, "mediatitle_maxsize") == 0) {
			currentskin.mediatitle_maxsize = atoi(value);
		} else if(strcmp(name, "mediaartist_xpos") == 0) {
			currentskin.mediaartist_xpos = atoi(value);
		} else if(strcmp(name, "mediaartist_ypos") == 0) {
			currentskin.mediaartist_ypos = atoi(value);
		} else if(strcmp(name, "mediaartist_centered") == 0) {
			currentskin.mediaartist_centered = atoi(value);
		} else if(strcmp(name, "mediaartist_maxsize") == 0) {
			currentskin.mediaartist_maxsize = atoi(value);
		} else if(strcmp(name, "mediaalbum_xpos") == 0) {
			currentskin.mediaalbum_xpos = atoi(value);
		} else if(strcmp(name, "mediaalbum_ypos") == 0) {
			currentskin.mediaalbum_ypos = atoi(value);
		} else if(strcmp(name, "mediaalbum_centered") == 0) {
			currentskin.mediaalbum_centered = atoi(value);
		} else if(strcmp(name, "mediaalbum_maxsize") == 0) {
			currentskin.mediaalbum_maxsize = atoi(value);
		} else if(strcmp(name, "title_mask") == 0) {
			currentskin.title_mask = atoi(value);
		} else if(strcmp(name, "menu_mask") == 0) {
			currentskin.menu_mask = atoi(value);
		} else if(strcmp(name, "selected_mask") == 0) {
			currentskin.selected_mask = atoi(value);
		} else if(strcmp(name, "position_mask") == 0) {
			currentskin.position_mask = atoi(value);
		} else if(strcmp(name, "mediatitle_mask") == 0) {
			currentskin.mediatitle_mask = atoi(value);
		} else if(strcmp(name, "mediaartist_mask") == 0) {
			currentskin.mediaartist_mask = atoi(value);
		} else if(strcmp(name, "mediaalbum_mask") == 0) {
			currentskin.mediaalbum_mask = atoi(value);
		} else if(strcmp(name, "time_xpos") == 0) {
			currentskin.time_xpos = atoi(value);
		} else if(strcmp(name, "time_ypos") == 0) {
			currentskin.time_ypos = atoi(value);
		} else if(strcmp(name, "time_mask") == 0) {
			currentskin.time_mask = atoi(value);
		}
	} while(pos < size);
}

void skin_init(char *path) {
	char *buffer;
	unzFile uf = NULL;
//	unz_file_info file_info;
	int err = UNZ_OK;

	if(currentskin.file != NULL) {
		free(currentskin.file);
		currentskin.file = NULL;
	}
	if(currentskin.main_bg != NULL) {
		free(currentskin.main_bg);
		currentskin.main_bg = NULL;
	}
	if(currentskin.sub_bg != NULL) {
		free(currentskin.sub_bg);
		currentskin.sub_bg = NULL;
	}
	if(currentskin.battery_icons != NULL) {
		free(currentskin.battery_icons);
		currentskin.battery_icons = NULL;
	}
	if(currentskin.state_icons != NULL) {
		free(currentskin.state_icons);
		currentskin.state_icons = NULL;
	}
	if(currentskin.hold_icons != NULL) {
		free(currentskin.hold_icons);
		currentskin.hold_icons = NULL;
	}
	if(currentskin.shuffle_icons != NULL) {
		free(currentskin.shuffle_icons);
		currentskin.shuffle_icons = NULL;
	}
	if(currentskin.volume_icons != NULL) {
		free(currentskin.volume_icons);
		currentskin.volume_icons = NULL;
	}
	if(currentskin.unknownicon != NULL) {
		free(currentskin.unknownicon);
		currentskin.unknownicon = NULL;
	}
	if(currentskin.selected != NULL) {
		free(currentskin.selected);
		currentskin.selected = NULL;
	}
	if(currentskin.seekbar_sprite != NULL) {
		free(currentskin.seekbar_sprite);
		currentskin.seekbar_sprite = NULL;
	}
	if(currentskin.seekbar_bar != NULL) {
		free(currentskin.seekbar_bar);
		currentskin.seekbar_bar = NULL;
	}
	if(currentskin.seekbar_full != NULL) {
		free(currentskin.seekbar_full);
		currentskin.seekbar_full = NULL;
	}
	if(currentskin.seekbar_empty != NULL) {
		free(currentskin.seekbar_empty);
		currentskin.seekbar_empty = NULL;
	}
	if(currentskin.progressbar_sprite != NULL) {
		free(currentskin.progressbar_sprite);
		currentskin.progressbar_sprite = NULL;
	}
	if(currentskin.progressbar_bar != NULL) {
		free(currentskin.progressbar_bar);
		currentskin.progressbar_bar = NULL;
	}
	if(currentskin.progressbar_full != NULL) {
		free(currentskin.progressbar_full);
		currentskin.progressbar_full = NULL;
	}
	if(currentskin.progressbar_empty != NULL) {
		free(currentskin.progressbar_empty);
		currentskin.progressbar_empty = NULL;
	}
	if(currentskin.volumebar_sprite != NULL) {
		free(currentskin.volumebar_sprite);
		currentskin.volumebar_sprite = NULL;
	}
	if(currentskin.volumebar_bar != NULL) {
		free(currentskin.volumebar_bar);
		currentskin.volumebar_bar = NULL;
	}
	if(currentskin.volumebar_full != NULL) {
		free(currentskin.volumebar_full);
		currentskin.volumebar_full = NULL;
	}
	if(currentskin.volumebar_empty != NULL) {
		free(currentskin.volumebar_empty);
		currentskin.volumebar_empty = NULL;
	}

	currentskin.file = strdup(path);

	buffer = (char *) malloc(32768 * sizeof(char));

	uf = unzOpen(path);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, "skin.conf", 0) != UNZ_OK)
		return;
	// err = unzGetCurrentFileInfo(uf, &file_info, filename, 256, NULL, 0, NULL, 0);
	err = unzOpenCurrentFile(uf);
	err = unzReadCurrentFile(uf, buffer, 32768);

	skin_parse(buffer, err);

	unzCloseCurrentFile(uf);

	free(buffer);
}

u16 **skin_get_font(void) {
	return font_myriad_web_9;
}

u16 skin_get_width(void) {
	return currentskin.width;
}

u16 skin_get_height(void) {
	return currentskin.height;
}

u16 skin_get_xpos(void) {
	return currentskin.xpos;
}

u16 skin_get_ypos(void) {
	return currentskin.ypos;
}

u16 *skin_get_main_bg(void) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.main_bg, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		s = (u16 *) malloc(256 * 192 * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 192; i++)
			for(j = 0; j < 256; j++)
				s[i * 256 + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(192 - i - 1) * 256 + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	return s;
}

u16 *skin_get_sub_bg(void) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.sub_bg, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		s = (u16 *) malloc(256 * 192 * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 192; i++)
			for(j = 0; j < 256; j++)
				s[i * 256 + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(192 - i - 1) * 256 + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	return s;
}

void skin_get_batteryicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.battery_icons, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		if(*p != NULL)
			free(*p);
		*p = (u16 *) malloc(3 * currentskin.battery_size * currentskin.battery_size * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 3 * currentskin.battery_size; i++)
			for(j = 0; j < currentskin.battery_size; j++)
				(*p)[i * currentskin.battery_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(3 * currentskin.battery_size - i - 1) * currentskin.battery_size + j]);
	}

	unzCloseCurrentFile(uf);

	free(buffer);

	*size = currentskin.battery_size;
	*ypos = currentskin.battery_ypos;
	*xpos = currentskin.battery_xpos;
}

void skin_get_stateicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.state_icons, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		if(*p != NULL)
			free(*p);
		*p = (u16 *) malloc(2 * currentskin.state_size * currentskin.state_size * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 2 * currentskin.state_size; i++)
			for(j = 0; j < currentskin.state_size; j++)
				(*p)[i * currentskin.state_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(2 * currentskin.state_size - i - 1) * currentskin.state_size + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	*size = currentskin.state_size;
	*ypos = currentskin.state_ypos;
	*xpos = currentskin.state_xpos;
}

void skin_get_holdicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.hold_icons, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		if(*p != NULL)
			free(*p);
		*p = (u16 *) malloc(2 * currentskin.hold_size * currentskin.hold_size * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 2 * currentskin.hold_size; i++)
			for(j = 0; j < currentskin.hold_size; j++)
				(*p)[i * currentskin.hold_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(2 * currentskin.hold_size - i - 1) * currentskin.hold_size + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	*size = currentskin.hold_size;
	*ypos = currentskin.hold_ypos;
	*xpos = currentskin.hold_xpos;
}

u16 * skin_get_unknownicon(void) {
	u16 *p, *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.unknownicon, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		p = (u16 *) malloc(64 * 64 * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 64; i++)
			for(j = 0; j < 64; j++)
				p[i * 64 + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(64 - i - 1) * 64 +  j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	return p;
}

void skin_get_shuffleicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.shuffle_icons, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		if(*p != NULL)
			free(*p);
		*p = (u16 *) malloc(currentskin.shuffle_size * currentskin.shuffle_size * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < currentskin.shuffle_size; i++)
			for(j = 0; j < currentskin.shuffle_size; j++)
				(*p)[i * currentskin.shuffle_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.shuffle_size - i - 1) * currentskin.shuffle_size + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	*size = currentskin.shuffle_size;
	*ypos = currentskin.shuffle_ypos;
	*xpos = currentskin.shuffle_xpos;
}

void skin_get_volumeicons(u16 **p, u16 *xpos, u16 *ypos, u16 *size) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.volume_icons, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		if(*p != NULL)
			free(*p);
		*p = (u16 *) malloc(currentskin.volume_size * currentskin.volume_size * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < currentskin.volume_size; i++)
			for(j = 0; j < currentskin.volume_size; j++)
				(*p)[i * currentskin.volume_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.volume_size - i - 1) * currentskin.volume_size + j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	*size = currentskin.volume_size;
	*ypos = currentskin.volume_ypos;
	*xpos = currentskin.volume_xpos;
}

u16 *skin_get_selected(void) {
	u16 *p, *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;
	if(unzLocateFile(uf, currentskin.selected, 0) != UNZ_OK)
		return;
	err = unzOpenCurrentFile(uf);

	buffer = (char *) malloc(131072);

	err = unzReadCurrentFile(uf, buffer, 131072);

	h = (struct bmheader *) buffer;
	ih = (struct bminfoheader *) (buffer + 14);

	if(ih->bits == 16) {
		int i, j;

		p = (u16 *) malloc(16 * 1 * 2);

		/* why the bitmap is stored upside down? */
		for(i = 0; i < 16; i++)
			for(j = 0; j < 1; j++)
				p[i * 1 + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(16 - i - 1) * 1 +  j]);
	}

	free(buffer);

	unzCloseCurrentFile(uf);

	return p;
}

void skin_get_seekbar(struct progressbar **p) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;

	if(*p != NULL) {
		if((*p)->type == 0) {
			free((*p)->full);
			free((*p)->empty);
		} else if((*p)->type == 1) {
			free((*p)->seekpos);
			free((*p)->seekbar);
		}
		free(*p);
	}

	*p = malloc(sizeof(struct progressbar));

	(*p)->type = currentskin.seekbar.type;

	(*p)->ypos = currentskin.seekbar.ypos;
	(*p)->xpos = currentskin.seekbar.xpos;
	(*p)->xsize = currentskin.seekbar.xsize;
	(*p)->ysize = currentskin.seekbar.ysize;

	if(currentskin.seekbar.type == 0) {
		if(unzLocateFile(uf, currentskin.seekbar_full, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->full = (u16 *) malloc(currentskin.seekbar.ysize * currentskin.seekbar.full_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.seekbar.ysize; i++)
				for(j = 0; j < currentskin.seekbar.full_size; j++)
					(*p)->full[i * currentskin.seekbar.full_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.seekbar.ysize - i - 1) * currentskin.seekbar.full_size + j]);
		}

		free(buffer);

		(*p)->full_size = currentskin.progressbar.full_size;
		(*p)->full_animate = currentskin.progressbar.full_animate;

		if(unzLocateFile(uf, currentskin.seekbar_empty, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->empty = (u16 *) malloc(currentskin.seekbar.ysize * currentskin.seekbar.empty_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.seekbar.ysize; i++)
				for(j = 0; j < currentskin.seekbar.empty_size; j++)
					(*p)->empty[i * currentskin.seekbar.empty_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.seekbar.ysize - i - 1) * currentskin.seekbar.empty_size + j]);
		}

		free(buffer);

		(*p)->empty_size = currentskin.progressbar.empty_size;
	} else if(currentskin.seekbar.type == 1) {
		if(unzLocateFile(uf, currentskin.seekbar_sprite, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekpos = (u16 *) malloc(currentskin.seekbar.seekpos_size * currentskin.seekbar.seekpos_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.seekbar.seekpos_size; i++)
				for(j = 0; j < currentskin.seekbar.seekpos_size; j++)
					(*p)->seekpos[i * currentskin.seekbar.seekpos_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.seekbar.seekpos_size - i - 1) * currentskin.seekbar.seekpos_size + j]);
		}

		free(buffer);

		(*p)->seekpos_ypos = currentskin.seekbar.seekpos_ypos;
		(*p)->seekpos_size = currentskin.seekbar.seekpos_size;

		if(unzLocateFile(uf, currentskin.seekbar_bar, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekbar = (u16 *) malloc(currentskin.seekbar.ysize * currentskin.seekbar.seekbar_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.seekbar.ysize; i++)
				for(j = 0; j < currentskin.seekbar.seekbar_size; j++)
					(*p)->seekbar[i * currentskin.seekbar.seekbar_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.seekbar.ysize - i - 1) * currentskin.seekbar.seekbar_size + j]);
		}

		free(buffer);

		(*p)->seekbar_size = currentskin.seekbar.seekbar_size;
	}

	unzCloseCurrentFile(uf);
}

void skin_get_progressbar(struct progressbar **p) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;

	if(*p != NULL) {
		if((*p)->type == 0) {
			free((*p)->full);
			free((*p)->empty);
		} else if((*p)->type == 1) {
			free((*p)->seekpos);
			free((*p)->seekbar);
		}
		free(*p);
	}

	*p = malloc(sizeof(struct progressbar));

	(*p)->type = currentskin.progressbar.type;

	(*p)->ypos = currentskin.progressbar.ypos;
	(*p)->xpos = currentskin.progressbar.xpos;
	(*p)->xsize = currentskin.progressbar.xsize;
	(*p)->ysize = currentskin.progressbar.ysize;

	if(currentskin.progressbar.type == 0) {
		if(unzLocateFile(uf, currentskin.progressbar_full, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->full = (u16 *) malloc(currentskin.progressbar.ysize * currentskin.progressbar.full_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.progressbar.ysize; i++)
				for(j = 0; j < currentskin.progressbar.full_size; j++)
					(*p)->full[i * currentskin.progressbar.full_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.progressbar.ysize - i - 1) * currentskin.progressbar.full_size + j]);
		}

		free(buffer);

		(*p)->full_size = currentskin.progressbar.full_size;
		(*p)->full_animate = currentskin.progressbar.full_animate;

		if(unzLocateFile(uf, currentskin.progressbar_empty, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->empty = (u16 *) malloc(currentskin.progressbar.ysize * currentskin.progressbar.empty_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.progressbar.ysize; i++)
				for(j = 0; j < currentskin.progressbar.empty_size; j++)
					(*p)->empty[i * currentskin.progressbar.empty_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.progressbar.ysize - i - 1) * currentskin.progressbar.empty_size + j]);
		}

		free(buffer);

		(*p)->empty_size = currentskin.progressbar.empty_size;
	} else if(currentskin.progressbar.type == 1) {
		if(unzLocateFile(uf, currentskin.progressbar_sprite, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekpos = (u16 *) malloc(currentskin.progressbar.seekpos_size * currentskin.progressbar.seekpos_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.progressbar.seekpos_size; i++)
				for(j = 0; j < currentskin.progressbar.seekpos_size; j++)
					(*p)->seekpos[i * currentskin.progressbar.seekpos_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.progressbar.seekpos_size - i - 1) * currentskin.progressbar.seekpos_size + j]);
		}

		free(buffer);

		(*p)->seekpos_ypos = currentskin.progressbar.seekpos_ypos;
		(*p)->seekpos_size = currentskin.progressbar.seekpos_size;

		if(unzLocateFile(uf, currentskin.progressbar_bar, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekbar = (u16 *) malloc(currentskin.progressbar.ysize * currentskin.progressbar.seekbar_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.progressbar.ysize; i++)
				for(j = 0; j < currentskin.progressbar.seekbar_size; j++)
					(*p)->seekbar[i * currentskin.progressbar.seekbar_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.progressbar.ysize - i - 1) * currentskin.progressbar.seekbar_size + j]);
		}

		free(buffer);

		(*p)->seekbar_size = currentskin.progressbar.seekbar_size;
	}

	unzCloseCurrentFile(uf);
}

void skin_get_volumebar(struct progressbar **p) {
	u16 *s;
	char *buffer;
	unzFile uf = NULL;
	int err = UNZ_OK;

	struct bmheader *h;
	struct bminfoheader *ih;

	uf = unzOpen(currentskin.file);
	if(uf == NULL)
		return;

	if(*p != NULL) {
		if((*p)->type == 0) {
			free((*p)->full);
			free((*p)->empty);
		} else if((*p)->type == 1) {
			free((*p)->seekpos);
			free((*p)->seekbar);
		}
		free(*p);
	}

	*p = malloc(sizeof(struct progressbar));

	(*p)->type = currentskin.volumebar.type;

	(*p)->ypos = currentskin.volumebar.ypos;
	(*p)->xpos = currentskin.volumebar.xpos;
	(*p)->xsize = currentskin.volumebar.xsize;
	(*p)->ysize = currentskin.volumebar.ysize;

	if(currentskin.volumebar.type == 0) {
		if(unzLocateFile(uf, currentskin.volumebar_full, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->full = (u16 *) malloc(currentskin.volumebar.ysize * currentskin.volumebar.full_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.volumebar.ysize; i++)
				for(j = 0; j < currentskin.volumebar.full_size; j++)
					(*p)->full[i * currentskin.volumebar.full_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.volumebar.ysize - i - 1) * currentskin.volumebar.full_size + j]);
		}

		free(buffer);

		(*p)->full_size = currentskin.volumebar.full_size;
		(*p)->full_animate = currentskin.volumebar.full_animate;

		if(unzLocateFile(uf, currentskin.volumebar_empty, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->empty = (u16 *) malloc(currentskin.volumebar.ysize * currentskin.volumebar.empty_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.volumebar.ysize; i++)
				for(j = 0; j < currentskin.volumebar.empty_size; j++)
					(*p)->empty[i * currentskin.volumebar.empty_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.volumebar.ysize - i - 1) * currentskin.volumebar.empty_size + j]);
		}

		free(buffer);

		(*p)->empty_size = currentskin.volumebar.empty_size;
	} else if(currentskin.volumebar.type == 1) {
		if(unzLocateFile(uf, currentskin.volumebar_sprite, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekpos = (u16 *) malloc(currentskin.volumebar.seekpos_size * currentskin.volumebar.seekpos_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.volumebar.seekpos_size; i++)
				for(j = 0; j < currentskin.volumebar.seekpos_size; j++)
					(*p)->seekpos[i * currentskin.volumebar.seekpos_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.volumebar.seekpos_size - i - 1) * currentskin.volumebar.seekpos_size + j]);
		}

		free(buffer);

		(*p)->seekpos_ypos = currentskin.volumebar.seekpos_ypos;
		(*p)->seekpos_size = currentskin.volumebar.seekpos_size;

		if(unzLocateFile(uf, currentskin.volumebar_bar, 0) != UNZ_OK)
			return;

		err = unzOpenCurrentFile(uf);

		buffer = (char *) malloc(131072);

		err = unzReadCurrentFile(uf, buffer, 131072);

		h = (struct bmheader *) buffer;
		ih = (struct bminfoheader *) (buffer + 14);

		if(ih->bits == 16) {
			int i, j;

			(*p)->seekbar = (u16 *) malloc(currentskin.volumebar.ysize * currentskin.volumebar.seekbar_size * 2);

			/* why the bitmap is stored upside down? */
			for(i = 0; i < currentskin.volumebar.ysize; i++)
				for(j = 0; j < currentskin.volumebar.seekbar_size; j++)
					(*p)->seekbar[i * currentskin.volumebar.seekbar_size + j] = RGBTOBGR(((u16 *) (buffer + h->offset))[(currentskin.volumebar.ysize - i - 1) * currentskin.volumebar.seekbar_size + j]);
		}

		free(buffer);

		(*p)->seekbar_size = currentskin.volumebar.seekbar_size;
	}

	unzCloseCurrentFile(uf);
}

void skin_get_positionpos(u16 *xpos, u16 *ypos, u16 *centered) {
	*xpos = currentskin.position_xpos;
	*ypos = currentskin.position_ypos;
	*centered = currentskin.position_centered;
}

void skin_get_albumpos(u16 *xpos, u16 *ypos, u16 *centered) {
	*xpos = currentskin.album_xpos;
	*ypos = currentskin.album_ypos;
	*centered = currentskin.album_centered;
}

void skin_get_titlepos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize) {
	*xpos = currentskin.title_xpos;
	*ypos = currentskin.title_ypos;
	*centered = currentskin.title_centered;
	*maxsize = currentskin.title_maxsize;
}

void skin_get_mtitlepos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize) {
	*xpos = currentskin.mediatitle_xpos;
	*ypos = currentskin.mediatitle_ypos;
	*centered = currentskin.mediatitle_centered;
	*maxsize = currentskin.mediatitle_maxsize;
}

void skin_get_martistpos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize) {
	*xpos = currentskin.mediaartist_xpos;
	*ypos = currentskin.mediaartist_ypos;
	*centered = currentskin.mediaartist_centered;
	*maxsize = currentskin.mediaartist_maxsize;
}

void skin_get_malbumpos(u16 *xpos, u16 *ypos, u16 *centered, u16 *maxsize) {
	*xpos = currentskin.mediaalbum_xpos;
	*ypos = currentskin.mediaalbum_ypos;
	*centered = currentskin.mediaalbum_centered;
	*maxsize = currentskin.mediaalbum_maxsize;
}
