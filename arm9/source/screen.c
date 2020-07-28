#include <fat.h>

#include <sys/dir.h>

#include <nds.h>
#include <nds/arm9/console.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <ipc2.h>

#define SCREEN_C

#include "playlist.h"
#include "screen.h"
#include "text.h"
#include "wheel.h"
#include "file.h"
#include "sound.h"
#include "input.h"
#include "tags.h"
#include "skin.h"

#define MAX(A,B) ((A) > (B) ? (A) : (B))

#define albumpic (((SpriteEntry*)OAM)[3])
#define volumepic (((SpriteEntry*)OAM)[4])
#define shufflepic (((SpriteEntry*)OAM)[5])
#define seekpospic (((SpriteEntry*)OAM)[6])
#define volumepospic (((SpriteEntry*)OAM)[7])
#define progresspospic (((SpriteEntry*)OAM)[8])

static uint16 *frontbuffer = (uint16 *)0x06020000,
			 *backbuffer = (uint16 *) 0x06040000;
static uint16 *backbuffer, *frontbuffer;
static char *screen_menu_name;
static u8 progress_offset;

static u16 screen_width, screen_height, screen_xpos, screen_ypos;
static uint16 *main_bg, *sub_bg;
static u16 **font;
static u16 *batteryicon, *batteryicons, battery_xpos, battery_ypos, battery_size;
static u16 *stateicon, *stateicons, state_xpos, state_ypos, state_size;
static u16 *holdicon, *holdicons, hold_xpos, hold_ypos, hold_size;
static u16 *albumicon, *unknownicon, album_xpos, album_ypos, album_size;
static u16 *volumeicon, *volumeicons, volume_xpos, volume_ypos, volume_size;
static u16 *shuffleicon, *shuffleicons, shuffle_xpos, shuffle_ypos, shuffle_size;
static u16 *screen_selected;
static u16 *seekposicon;
static struct progressbar *seekbar;
static u16 *progressposicon;
static struct progressbar *progressbar;
static u16 *volumeposicon;
static struct progressbar *volumebar;

static u16 position_xpos, position_ypos, position_centered;
static u16 album_xpos, album_ypos, album_centered;
static u16 title_xpos, title_ypos, title_centered, title_maxsize;
static u16 mtitle_xpos, mtitle_ypos, mtitle_centered, mtitle_maxsize;
static u16 martist_xpos, martist_ypos, martist_centered, martist_maxsize;
static u16 malbum_xpos, malbum_ypos, malbum_centered, malbum_maxsize;

static struct screen_scrolling_text *screen_scroll_title = &screen_scrolling_text[0],
																		*screen_scroll_artist = &screen_scrolling_text[1],
																		*screen_scroll_album = &screen_scrolling_text[2];

static void screen_reset_backbuffer(void) {
	dmaCopy(main_bg, backbuffer, 256 * 192 * 2);
}

static void screen_switchbuffers(void) {
	uint16 *t;

	t = backbuffer;
	backbuffer = frontbuffer;
	frontbuffer = t;

	BG2_CR ^= BG_PRIORITY_1;
	BG2_CR ^= BG_PRIORITY_2;
	BG3_CR ^= BG_PRIORITY_1;
	BG3_CR ^= BG_PRIORITY_2;
}

static void screen_initsprites(void) {
	SpriteEntry *spr_batt,
							*spr_state,
							*spr_hold,
							*spr_album,
							*spr_vol,
							*spr_shuffle,
							*spr_seekpos,
							*spr_progresspos,
							*spr_volumepos;
	u32 nextpos = 0;
	int i;
	SpriteEntry *s;

	s = (SpriteEntry *) OAM;
	for(i = 0; i < 128; i++) {
		s[i].attribute[0] = ATTR0_DISABLED;
		s[i].attribute[1] = 0;
		s[i].attribute[2] = 0;
	}

	spr_batt = &((SpriteEntry*)OAM)[0];
	spr_batt->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (battery_ypos);
	spr_batt->attribute[2] = ATTR2_ALPHA(1) | 0;
	batteryicon = ((u16 *) SPRITE_GFX);
	switch(battery_size) {
		case 8:
			spr_batt->attribute[1] = ATTR1_SIZE_8 | (battery_xpos);
			nextpos += 8*8 * 2;
			break;

		case 16:
			spr_batt->attribute[1] = ATTR1_SIZE_16 | (battery_xpos);
			nextpos += 16*16 * 2;
			break;

		case 32:
			spr_batt->attribute[1] = ATTR1_SIZE_32 | (battery_xpos);
			nextpos += 32*32 * 2;
			break;

		case 64:
			spr_batt->attribute[1] = ATTR1_SIZE_64 | (battery_xpos);
			nextpos += 64*64 * 2;
			break;

		default:
			break;
	}

	spr_state = &((SpriteEntry*)OAM)[1];
	spr_state->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (state_ypos);
	spr_state->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
	stateicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
	switch(state_size) {
		case 8:
			spr_state->attribute[1] = ATTR1_SIZE_8 | (state_xpos);
			nextpos += 8*8 * 2;
			break;

		case 16:
			spr_state->attribute[1] = ATTR1_SIZE_16 | (state_xpos);
			nextpos += 16*16 * 2;
			break;

		case 32:
			spr_state->attribute[1] = ATTR1_SIZE_32 | (state_xpos);
			nextpos += 32*32 * 2;
			break;

		case 64:
			spr_state->attribute[1] = ATTR1_SIZE_64 | (state_xpos);
			nextpos += 64*64 * 2;
			break;

		default:
			break;
	}

	spr_hold = &((SpriteEntry*)OAM)[2];
	spr_hold->attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (hold_ypos);
	spr_hold->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
	holdicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
	switch(hold_size) {
		case 8:
			spr_hold->attribute[1] = ATTR1_SIZE_8 | (hold_xpos);
			nextpos += 8*8 * 2;
			break;

		case 16:
			spr_hold->attribute[1] = ATTR1_SIZE_16 | (hold_xpos);
			nextpos += 16*16 * 2;
			break;

		case 32:
			spr_hold->attribute[1] = ATTR1_SIZE_32 | (hold_xpos);
			nextpos += 32*32 * 2;
			break;

		case 64:
			spr_hold->attribute[1] = ATTR1_SIZE_64 | (hold_xpos);
			nextpos += 64*64 * 2;
			break;

		default:
			break;
	}

	spr_album = &((SpriteEntry*)OAM)[3];
	spr_album->attribute[0] = ATTR0_DISABLED;
	spr_album->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
	albumicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
	if(album_centered == 1)
		spr_album->attribute[1] = ATTR1_SIZE_64 | 96;
	else
		spr_album->attribute[1] = ATTR1_SIZE_64 | (album_xpos);
	nextpos += 64 * 64 * 2;

	spr_vol = &((SpriteEntry*)OAM)[4];
	spr_vol->attribute[0] = ATTR0_DISABLED;
	spr_vol->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
	volumeicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
	switch(volume_size) {
		case 8:
			spr_vol->attribute[1] = ATTR1_SIZE_8 | (volume_xpos);
			nextpos += 8*8 * 2;
			break;

		case 16:
			spr_vol->attribute[1] = ATTR1_SIZE_16 | (volume_xpos);
			nextpos += 16*16 * 2;
			break;

		case 32:
			spr_vol->attribute[1] = ATTR1_SIZE_32 | (volume_xpos);
			nextpos += 32*32 * 2;
			break;

		case 64:
			spr_vol->attribute[1] = ATTR1_SIZE_64 | (volume_xpos);
			nextpos += 64*64 * 2;
			break;

		default:
			break;
	}

	spr_shuffle = &((SpriteEntry*)OAM)[5];
	spr_shuffle->attribute[0] = ATTR0_DISABLED;
	spr_shuffle->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
	shuffleicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
	switch(shuffle_size) {
		case 8:
			spr_shuffle->attribute[1] = ATTR1_SIZE_8 | (shuffle_xpos);
			nextpos += 8*8 * 2;
			break;

		case 16:
			spr_shuffle->attribute[1] = ATTR1_SIZE_16 | (shuffle_xpos);
			nextpos += 16*16 * 2;
			break;

		case 32:
			spr_shuffle->attribute[1] = ATTR1_SIZE_32 | (shuffle_xpos);
			nextpos += 32*32 * 2;
			break;

		case 64:
			spr_shuffle->attribute[1] = ATTR1_SIZE_64 | (shuffle_xpos);
			nextpos += 64*64 * 2;
			break;

		default:
			break;
	}

	if(seekbar->type == 1) {
		spr_seekpos = &((SpriteEntry*)OAM)[6];
		spr_seekpos->attribute[0] = ATTR0_DISABLED;
		spr_seekpos->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
		seekposicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
		switch(seekbar->seekpos_size) {
			case 8:
				spr_seekpos->attribute[1] = ATTR1_SIZE_8;
				nextpos += 8*8 * 2;
				break;

			case 16:
				spr_seekpos->attribute[1] = ATTR1_SIZE_16;
				nextpos += 16*16 * 2;
				break;

			case 32:
				spr_seekpos->attribute[1] = ATTR1_SIZE_32;
				nextpos += 32*32 * 2;
				break;

			case 64:
				spr_seekpos->attribute[1] = ATTR1_SIZE_64;
				nextpos += 64*64 * 2;
				break;

			default:
				break;
		}
	}

	if(progressbar->type == 1) {
		spr_progresspos = &((SpriteEntry*)OAM)[7];
		spr_progresspos->attribute[0] = ATTR0_DISABLED;
		spr_progresspos->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
		progressposicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
		switch(progressbar->seekpos_size) {
			case 8:
				spr_progresspos->attribute[1] = ATTR1_SIZE_8;
				nextpos += 8*8 * 2;
				break;

			case 16:
				spr_progresspos->attribute[1] = ATTR1_SIZE_16;
				nextpos += 16*16 * 2;
				break;

			case 32:
				spr_progresspos->attribute[1] = ATTR1_SIZE_32;
				nextpos += 32*32 * 2;
				break;

			case 64:
				spr_progresspos->attribute[1] = ATTR1_SIZE_64;
				nextpos += 64*64 * 2;
				break;

			default:
				break;
		}
	}

	if(volumebar->type == 1) {
		spr_volumepos = &((SpriteEntry*)OAM)[6];
		spr_volumepos->attribute[0] = ATTR0_DISABLED;
		spr_volumepos->attribute[2] = ATTR2_ALPHA(1) | nextpos / 128;
		volumeposicon = ((u16 *) SPRITE_GFX) + nextpos / 2;
		switch(seekbar->seekpos_size) {
			case 8:
				spr_volumepos->attribute[1] = ATTR1_SIZE_8;
				nextpos += 8*8 * 2;
				break;

			case 16:
				spr_volumepos->attribute[1] = ATTR1_SIZE_16;
				nextpos += 16*16 * 2;
				break;

			case 32:
				spr_volumepos->attribute[1] = ATTR1_SIZE_32;
				nextpos += 32*32 * 2;
				break;

			case 64:
				spr_volumepos->attribute[1] = ATTR1_SIZE_64;
				nextpos += 64*64 * 2;
				break;

			default:
				break;
		}
	}

}

static void screen_load_skin(char *p) {
	skin_init(p);

	if(main_bg != NULL)
		free(main_bg);
	main_bg = skin_get_main_bg();

	if(sub_bg != NULL)
		free(sub_bg);
	sub_bg = skin_get_sub_bg();

	font = skin_get_font();

	screen_xpos = skin_get_xpos();
	screen_ypos = skin_get_ypos();
	screen_height = skin_get_height();
	screen_width = skin_get_width();

	skin_get_batteryicons(&batteryicons, &battery_xpos, &battery_ypos, &battery_size);

	skin_get_stateicons(&stateicons, &state_xpos, &state_ypos, &state_size);

	skin_get_holdicons(&holdicons, &hold_xpos, &hold_ypos, &hold_size);

	if(unknownicon != NULL)
		free(unknownicon);
	unknownicon = skin_get_unknownicon();

	skin_get_shuffleicons(&shuffleicons, &shuffle_xpos, &shuffle_ypos, &shuffle_size);

	skin_get_volumeicons(&volumeicons, &volume_xpos, &volume_ypos, &volume_size);

	if(screen_selected != NULL)
		free(screen_selected);
	screen_selected = skin_get_selected();

	skin_get_seekbar(&seekbar);
	skin_get_progressbar(&progressbar);
	skin_get_volumebar(&volumebar);

	skin_get_positionpos(&position_xpos, &position_ypos, &position_centered);
	skin_get_albumpos(&album_xpos, &album_ypos, &album_centered);
	skin_get_titlepos(&title_xpos, &title_ypos, &title_centered, &title_maxsize);
	skin_get_mtitlepos(&mtitle_xpos, &mtitle_ypos, &mtitle_centered, &mtitle_maxsize);
	skin_get_martistpos(&martist_xpos, &martist_ypos, &martist_centered, &martist_maxsize);
	skin_get_malbumpos(&malbum_xpos, &malbum_ypos, &malbum_centered, &malbum_maxsize);
}

void screen_initdisplays(void) {
	int i, j;
	const char *starting = "Starting...";

	/* setup main display */
	videoSetMode(MODE_5_2D | DISPLAY_BG2_ACTIVE | DISPLAY_BG3_ACTIVE
		| DISPLAY_SPR_ACTIVE | DISPLAY_SPR_1D_LAYOUT | DISPLAY_SPR_1D_BMP);

	vramSetBankA(VRAM_A_MAIN_BG_0x06000000);
	vramSetBankB(VRAM_B_MAIN_BG_0x06020000);
	vramSetBankD(VRAM_D_MAIN_BG_0x06040000);
	vramSetBankE(VRAM_E_MAIN_SPRITE);

	/* console */
	BG0_CR = BG_MAP_BASE(31);
	BG_PALETTE[255] = RGB15(31,31,31);
	consoleInitDefault((u16*)SCREEN_BASE_BLOCK(31), (u16*)CHAR_BASE_BLOCK(0), 16);

	/* background */
	BG2_CR = BG_BMP16_256x256 | BG_BMP_BASE(128 / 16) | BG_PRIORITY_1;
	BG2_XDX = 1 << 8;
	BG2_XDY = 0;
	BG2_YDX = 0;
	BG2_YDY = 1 << 8;
	BG2_CX = 0;
	BG2_CY = 0;

	/* screen */
	BG3_CR = BG_BMP16_256x256 | BG_BMP_BASE(256 / 16) | BG_PRIORITY_2;
	BG3_XDX = 1 << 8;
	BG3_XDY = 0;
	BG3_YDX = 0;
	BG3_YDY = 1 << 8;
	BG3_CX = 8;
	BG3_CY = 8;

	/* setup sub display */
	videoSetModeSub(MODE_5_2D | DISPLAY_BG3_ACTIVE);
	vramSetBankC(VRAM_C_SUB_BG);
	SUB_BG3_CR = BG_BMP16_256x256;

	/* scaling and rotating */
	SUB_BG3_XDX = 1 << 8;
	SUB_BG3_XDY = 0;
	SUB_BG3_YDX = 0;
	SUB_BG3_YDY = 1 << 8;

	/* translation */
	SUB_BG3_CX = 0;
	SUB_BG3_CY = 0;

	screen_load_skin("/lmp-ng/skins/default.zip");
	screen_initsprites();

	DC_FlushAll();

	dmaCopy(sub_bg, BG_GFX_SUB, 256*192*2);
	dmaCopy(main_bg, frontbuffer, 256*192*2);
	dmaCopy(main_bg, backbuffer, 256*192*2);

	dmaCopy(unknownicon, albumicon, 64 * 64 * 2);
	dmaCopy(volumeicons, volumeicon, volume_size * volume_size * 2);
	dmaCopy(shuffleicons, shuffleicon, shuffle_size * shuffle_size * 2);
	if(seekbar->type == 1)
		dmaCopy(seekbar->seekpos, seekposicon, seekbar->seekpos_size * seekbar->seekpos_size * 2);
	if(progressbar->type == 1)
		dmaCopy(progressbar->seekpos, seekposicon, progressbar->seekpos_size * progressbar->seekpos_size * 2);
	if(volumebar->type == 1)
		dmaCopy(volumebar->seekpos, seekposicon, volumebar->seekpos_size * volumebar->seekpos_size * 2);

	screen_switchbuffers();
}

static void screen_update_battery(void) {
	static uint16 battery = 0x7fff, aux = 0x7fff;
	int i, j;

	u32 x, y;

	if(battery == IPC->battery && aux == IPC->aux)
		return;

	battery = IPC->battery;
	aux = IPC->aux;

	if(battery == 0 && aux == 0) {
		dmaCopy(batteryicons, batteryicon, battery_size * battery_size * 2);
	} else if(battery == 0 && aux != 0)
		dmaCopy(batteryicons + 2 * battery_size * battery_size, batteryicon, battery_size * battery_size * 2);
	else
		dmaCopy(batteryicons + battery_size * battery_size, batteryicon, battery_size * battery_size * 2);
}

static void screen_update_state(void) {
	static u8 state = 0xff;

	if(state == IPC2->sound_state)
		return;

	state = IPC2->sound_state;

	switch(state) {
		case IPC2_PLAYING:
			dmaCopy(stateicons, stateicon, state_size * state_size * 2);
			break;

		default:
		case IPC2_STOPPED:
			dmaCopy(stateicons + state_size * state_size, stateicon, state_size * state_size * 2);
			break;
	}
}

static void screen_update_hold(void) {
	static u8 hold = 0xff;

	if(hold == input_hold)
		return;

	hold = input_hold;

	switch(hold) {
		default:
		case INPUT_HOLD_OFF:
			memset(holdicon, 0, hold_size * hold_size * 2);
			break;

		case INPUT_HOLD_NORMAL:
			dmaCopy(holdicons, holdicon, hold_size * hold_size * 2);
			break;

		case INPUT_HOLD_ALL:
			dmaCopy(holdicons + hold_size * hold_size, holdicon, hold_size * hold_size * 2);
			break;
	}
}

static int screen_drawmenu(const struct screen_menu_entry *m) {
	int i, j;
	struct screen_scrolling_text *s = &screen_scrolling_text[3];
	int t;

	if(screen_menu_name != NULL) {

		if(title_centered == 1) {
			int w;

			w = getStringWidth(screen_menu_name, font);

			if(screen_width/2 - w/2 < title_maxsize / 2)
				w = title_maxsize / 2;
			else
				w = screen_width/2 - w/2;

			title_xpos = screen_xpos + w;
		}

		dispString2(title_xpos,
			title_ypos,
			currentskin.title_mask,
			screen_menu_name, backbuffer, font,
			screen_width + screen_xpos - 40,
			screen_height + screen_ypos,
			256);
	}

	if(screen_menu_selected > screen_menu_startentry && screen_menu_selected - screen_menu_startentry >= screen_height/16 - 2)
		screen_menu_startentry = screen_menu_selected - screen_height/16 + 2;
	else if(screen_menu_startentry > screen_menu_selected)
		screen_menu_startentry = screen_menu_selected;

	for(i = screen_menu_startentry; i < screen_menu_items && i - screen_menu_startentry < screen_height/16 - 1; i++) {

		dispString2(screen_xpos + 3,
			screen_ypos + 16 + (i - screen_menu_startentry) * 16,
			currentskin.menu_mask,
			m[i].label, backbuffer, font,
			screen_xpos + screen_width - 3 - 12, screen_ypos + screen_height,
			256);

		if(m[i].have_submenu == 1)
			dispString2(screen_xpos + screen_width - 12,
				screen_ypos + 16 + (i - screen_menu_startentry) * 16,
				currentskin.menu_mask,
				">",
				backbuffer, font, screen_width + screen_xpos - 3, screen_height + screen_ypos, 256);
	}


	for(i = 0; i <16; i++)
		for(j = 0; j < screen_width; j++)
			backbuffer[(i + 16 * (screen_menu_selected - screen_menu_startentry + 1) + screen_ypos) * 256 + screen_xpos + j] = ((u16 *) screen_selected)[i];

	t = screen_menu_selected - screen_menu_startentry;

	if(s->data != NULL) {
		free(s->data);
		s->data = NULL;
	}
	s->animate = 0;


	s->width = getStringWidth(m[screen_menu_selected].label, font) + 1;
	s->height = getStringHeight(m[screen_menu_selected].label, font) + 1;

	if(s->width > screen_width - 3 - 3 - 12) {
		s->animate = 1;

		s->data = (u16 *) malloc(s->width * s->height * 2);
		for(i = 0; i < s->height; i++)
			for(j = 0; j < s->width; j++)
				s->data[i * s->width + j] = ((u16 *) screen_selected)[i];

		s->background = (u16 *) screen_selected;
		s->bg_width = 1;

		dispString(0, 0, currentskin.selected_mask,
			m[screen_menu_selected].label, s->data, font,
			s->width, s->height,
			s->width);

		s->x_offset = 0;
		s->y = 16 + screen_ypos + t * 16;
		s->x = 3 + screen_xpos;
		s->delay = 0;
		s->maxwidth = screen_xpos + screen_width - 3 - 12;

	} else {
		dispString(screen_xpos + 3, screen_ypos + 16 *(screen_menu_selected - screen_menu_startentry + 1), currentskin.selected_mask,
			m[screen_menu_selected].label,
			backbuffer, font, screen_width + screen_xpos, screen_height + screen_ypos, 256);
	}

	if(m[screen_menu_selected].have_submenu == 1)
		dispString(screen_xpos + screen_width - 12, screen_ypos + 16 *(screen_menu_selected - screen_menu_startentry + 1), currentskin.selected_mask,
			">",
			backbuffer, font, screen_width + screen_xpos, screen_height + screen_ypos, 256);

	screen_switchbuffers();
}

static struct screen_menu_entry *screen_playlist_menu;
void screen_playlistmenu(struct playlist *p) {
	int i;

	if(screen_playlist_menu != NULL) {
		free(screen_playlist_menu);
		screen_playlist_menu = NULL;
	}

	screen_playlist_menu = (struct screen_menu_entry *) malloc(p->size * sizeof(struct screen_menu_entry));

	for(i = 0; i < p->size; i++) {
		screen_playlist_menu[i].label = p->list[i]->title;
		screen_playlist_menu[i].have_submenu = 0;
		screen_playlist_menu[i].callback = input_playlist;
	}

	screen_type = MENU;
	screen_menu = LIST;

	if(p->name != NULL)
		screen_menu_name = p->name;
	else
		screen_menu_name = "";

	screen_menu_items = p->size;
	if(screen_menu_startentry < 0 || screen_menu_startentry > p->size)
		screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = playlist_input_p;
	currentmenu = screen_playlist_menu;
}

static struct screen_menu_entry *screen_playlists_menu;
void screen_playlistsmenu(void) {
	int i;
	int size;

	if(screen_playlists_menu != NULL) {
		free(screen_playlists_menu);
		screen_playlists_menu = NULL;
	}

	screen_playlists_menu = (struct screen_menu_entry *) malloc(playlists.size * sizeof(struct screen_menu_entry));

	size = 0;
	for(i = 0; i < playlists.size; i++) {
		if(playlists.p[i]->name == NULL)
			continue;

		screen_playlists_menu[size].label = playlists.p[i]->name;
		screen_playlists_menu[size].have_submenu = 0;
		screen_playlists_menu[size].callback = input_playlists;
		size++;
	}

	screen_type = MENU;
	screen_menu = LISTS;

	screen_menu_name = "Playlists";

	screen_menu_items = size;
	if(screen_menu_startentry < 0 || screen_menu_startentry > size)
		screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = playlists_input_p;
	currentmenu = screen_playlists_menu;
}

static struct screen_menu_entry music_menu[] = {
	{ "Songs", 1, input_songs_setup },
	{ "Playlists", 1, input_playlists_setup },
	{ "Now Playing", 1, input_nowplaying_setup }
};
static char music_menu_name[] = "Music";
void screen_musicmenu(void) {
	screen_type = MENU;
	screen_menu = MUSIC;

	screen_menu_name = music_menu_name;

	screen_menu_items = sizeof(music_menu) / sizeof(struct screen_menu_entry);
	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = music_input_p;
	currentmenu = music_menu;
}

void screen_skinsmenu(void);

void screen_load_skin_callback(int pos, const char *text) {
	char file[512];

	strcpy(file, "/lmp-ng/skins/");
	strcat(file, text);

	screen_load_skin(file);
	screen_initsprites();

	DC_FlushAll();

	dmaCopy(sub_bg, BG_GFX_SUB, 256*192*2);
	dmaCopy(main_bg, frontbuffer, 256*192*2);
	dmaCopy(main_bg, backbuffer, 256*192*2);

	dmaCopy(unknownicon, albumicon, 64 * 64 * 2);
	dmaCopy(volumeicons, volumeicon, volume_size * volume_size * 2);
	dmaCopy(shuffleicons, shuffleicon, shuffle_size * shuffle_size * 2);

	if(seekbar->type == 1)
		dmaCopy(seekbar->seekpos, seekposicon, seekbar->seekpos_size * seekbar->seekpos_size * 2);
	if(progressbar->type == 1)
		dmaCopy(progressbar->seekpos, seekposicon, progressbar->seekpos_size * progressbar->seekpos_size * 2);
	if(volumebar->type == 1)
		dmaCopy(volumebar->seekpos, seekposicon, volumebar->seekpos_size * volumebar->seekpos_size * 2);

	screen_skinsmenu();
	screen_reset_backbuffer();
	screen_drawmenu(currentmenu);
	screen_reset_backbuffer();
	screen_drawmenu(currentmenu);
}

static struct screen_menu_entry *screen_skins_menu;
void screen_skinsmenu(void) {
	static int size = 0;

	screen_type = MENU;
	screen_menu = SKINS;

	if(screen_skins_menu == NULL) {

		screen_skins_menu = (struct screen_menu_entry *) malloc(128 * sizeof(struct screen_menu_entry));

		/* scan /lmp-ng/skins for skins */
		{
			DIR_ITER *d;
			char filename[256];
			struct stat s;

			d = diropen("/lmp-ng/skins");

			while(dirnext(d, filename, &s) != -1) {
				if(strcmp(filename, ".") == 0 || strcmp(filename, "..") == 0)
					continue;

				if(S_ISREG(s.st_mode)) {
					int len = strlen(filename);

					if(len > 3 && tolower(filename[len-1]) == 'p' && tolower(filename[len-2]) == 'i' && tolower(filename[len-3]) == 'z') {
						screen_skins_menu[size].label = strdup(filename);
						screen_skins_menu[size].have_submenu = 0;
						screen_skins_menu[size].callback = screen_load_skin_callback;
						size++;
					}
				}
			}
		}
	}

	screen_menu_name = "Skins";
	screen_menu_items = size;

	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = skins_input_p;
	currentmenu = screen_skins_menu;
}

static struct screen_menu_entry settings_menu[] = {
	{ "Scan metadata (slow)", 0, input_tags_scan },
	{ "Skins", 1, input_skins_setup },
};
void screen_settingsmenu(void) {
	screen_type = MENU;
	screen_menu = SETTINGS;

	screen_menu_name = "Settings";

	screen_menu_items = sizeof(settings_menu) / sizeof(struct screen_menu_entry);
	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = settings_input_p;
	currentmenu = settings_menu;
}

static struct screen_menu_entry main_menu[] = {
	{ "Music", 1, input_musicmenu_setup },
	{ "Shuffle Songs", 0, input_shuffle_setup },
//	{ "Photos", 1, NULL },
//	{ "Videos", 1, NULL },
	{ "Settings", 1, input_settings_setup },
	{ "Now Playing", 1, input_nowplaying_setup }
};

void screen_mainmenu(void) {
	screen_type = MENU;
	screen_menu = MAIN;

	screen_menu_name = NULL;

	screen_menu_items = sizeof(main_menu) / sizeof(struct screen_menu_entry);
	screen_menu_startentry = 0;
	screen_menu_selected = 0;
	currenthandle = menu_input_p;
	currentmenu = main_menu;
}

void screen_nowplaying(void) {
	char *playing = "Now Playing";
	char buffer[128];
	int i, j;

	screen_type = NOW_PLAYING;

	currenthandle = nowplaying_input_p;

	if(title_centered == 1) {
		int w;

		w = getStringWidth(playing, font);

		if(screen_width/2 - w/2 < title_maxsize / 2)
			w = title_maxsize / 2;
		else
			w = screen_width/2 - w/2;

		title_xpos = screen_xpos + w;
	}
	dispString(title_xpos,
		title_ypos,
		currentskin.title_mask, playing, backbuffer, font,
		screen_width + screen_xpos,
		screen_height + screen_ypos,
		256);

	albumpic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (album_ypos);

	if(playlists.p[playlists.current] == shufflelist)
		shufflepic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (shuffle_ypos);

	sprintf(buffer, "%d of %d", PLAYLIST->current + 1, PLAYLIST->size);
	if(position_centered == 1) {
		int w;

		w = getStringWidth(buffer, font);

		position_xpos = screen_xpos + screen_width/2 - w/2;
	}
	dispString(position_xpos,
		position_ypos,
		currentskin.position_mask, buffer, backbuffer, font,
		screen_width + screen_xpos,
		screen_height + screen_ypos,
		256);

	if(currentmedia->title != NULL) {
		if(screen_scroll_title->data != NULL) {
			free(screen_scroll_title->data);
			screen_scroll_title->data = NULL;
		}

		screen_scroll_title->width = getStringWidth(currentmedia->title, font) + 1;
		screen_scroll_title->height = getStringHeight(currentmedia->title, font) + 1;

		if(screen_scroll_title->width > mtitle_maxsize) {
			screen_scroll_title->animate = 1;

			screen_scroll_title->data = (u16 *) malloc(screen_scroll_title->width * screen_scroll_title->height * 2);
			for(i = 0; i < screen_scroll_title->height; i++)
				for(j = 0; j < screen_scroll_title->width; j++)
					screen_scroll_title->data[i * screen_scroll_title->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, currentskin.mediatitle_mask,
				currentmedia->title, screen_scroll_title->data, font,
				screen_scroll_title->width, screen_scroll_title->height,
				screen_scroll_title->width);

			screen_scroll_title->x_offset = 0;
			screen_scroll_title->y = mtitle_ypos;
			if(mtitle_centered == 0)
				screen_scroll_title->x = mtitle_xpos;
			else
				screen_scroll_title->x = screen_width/2 - screen_scroll_title->width/2;
			screen_scroll_title->delay = 0;
			screen_scroll_title->maxwidth = mtitle_xpos + mtitle_maxsize;

		} else {
			screen_scroll_title->animate = 0;

			dispString(mtitle_xpos, mtitle_ypos, currentskin.mediatitle_mask,
				currentmedia->title, backbuffer, font,
				screen_xpos + screen_width, screen_ypos + screen_height,
				256);
		}
	}

	if(currentmedia->artist != NULL) {
		if(screen_scroll_artist->data != NULL) {
			free(screen_scroll_artist->data);
			screen_scroll_artist->data = NULL;
		}

		screen_scroll_artist->width = getStringWidth(currentmedia->artist, font) + 1;
		screen_scroll_artist->height = getStringHeight(currentmedia->artist, font) + 1;

		if(screen_scroll_artist->width > martist_maxsize) {
			screen_scroll_artist->animate = 1;

			screen_scroll_artist->data = (u16 *) malloc(screen_scroll_artist->width * screen_scroll_artist->height * 2);
			for(i = 0; i < screen_scroll_artist->height; i++)
				for(j = 0; j < screen_scroll_artist->width; j++)
					screen_scroll_artist->data[i * screen_scroll_artist->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, currentskin.mediaartist_mask,
				currentmedia->artist, screen_scroll_artist->data, font,
				screen_scroll_artist->width, screen_scroll_artist->height,
				screen_scroll_artist->width);

			screen_scroll_artist->x_offset = 0;
			screen_scroll_artist->y = martist_ypos;
			if(martist_centered == 0)
				screen_scroll_artist->x = martist_xpos;
			else
				screen_scroll_artist->x = screen_width / 2 - screen_scroll_artist->width / 2;
			screen_scroll_artist->delay = 0;
			screen_scroll_artist->maxwidth = martist_xpos + martist_maxsize;

		} else {
			screen_scroll_artist->animate = 0;

			dispString(martist_xpos, martist_ypos, currentskin.mediaartist_mask,
				currentmedia->artist, backbuffer, font,
				screen_xpos + screen_width, screen_ypos + screen_height,
				256);
		}
	}

	if(currentmedia->album != NULL) {
		if(screen_scroll_album->data != NULL) {
			free(screen_scroll_album->data);
			screen_scroll_album->data = NULL;
		}

		screen_scroll_album->width = getStringWidth(currentmedia->album, font) + 1;
		screen_scroll_album->height = getStringHeight(currentmedia->album, font) + 1;

		if(screen_scroll_album->width > malbum_maxsize) {

			screen_scroll_album->animate = 1;

			screen_scroll_album->data = (u16 *) malloc(screen_scroll_album->width * screen_scroll_album->height * 2);
			for(i = 0; i < screen_scroll_album->height; i++)
				for(j = 0; j < screen_scroll_album->width; j++)
					screen_scroll_album->data[i * screen_scroll_album->width + j] = RGB15(31,31,31) | BIT(15);

			dispString(0, 0, currentskin.mediaalbum_mask,
				currentmedia->album, screen_scroll_album->data, font,
				screen_scroll_album->width, screen_scroll_album->height,
				screen_scroll_album->width);

			screen_scroll_album->x_offset = 0;
			screen_scroll_album->y = malbum_ypos;
			if(mtitle_centered == 0)
				screen_scroll_album->x = malbum_xpos;
			else
				screen_scroll_album->x = screen_width/2 - screen_scroll_album->width / 2;
			screen_scroll_album->delay = 0;
			screen_scroll_album->maxwidth = malbum_xpos + malbum_maxsize;

		} else {
			screen_scroll_album->animate = 0;

			dispString(malbum_xpos, malbum_ypos, currentskin.mediaalbum_mask,
				currentmedia->album, backbuffer, font,
				screen_xpos + screen_width, screen_ypos + screen_height,
				256);
		}
	}

	screen_switchbuffers();
}

void screen_nowplaying_reset(void) {
	if(screen_scroll_title != NULL)
		screen_scroll_title->animate = 0;
	if(screen_scroll_artist != NULL)
		screen_scroll_artist->animate = 0;
	if(screen_scroll_album != NULL)
		screen_scroll_album->animate = 0;

	screen_reset_backbuffer();
	screen_nowplaying();
	screen_reset_backbuffer();
	screen_nowplaying();
}

static void screen_animate_update(void) {
	int i, j, k;
	int offset;
	struct screen_scrolling_text *s = screen_scrolling_text;

	for(k = 0; k < SCREEN_MAX_SCROLLING_TEXT; k++) {
		if(s[k].animate == 0)
			continue;

		offset = s[k].x_offset/2;

		if(offset == 0 && s[k].delay == 0)
			s[k].delay = 60;

		if(s[k].delay != 0)
			s[k].delay--;

		if(s[k].delay == 0)
			s[k].x_offset++;

		if(s[k].x_offset >= 2 * s[k].width)
			s[k].x_offset = 0;

		for(i = 0; i < s[k].height; i++)
			for(j = 0; j + offset < s[k].width && s[k].x + j < s[k].maxwidth; j++)
				backbuffer[(i + s[k].y) * 256 + s[k].x + j] = s[k].data[i * s[k].width + j + offset];

		if(s[k].background == NULL)
			for(i = 0; i < s[k].height; i++)
				for(j = s[k].width - offset; j < s[k].width && s[k].x + j < s[k].maxwidth; j++)
					backbuffer[(i + s[k].y) * 256 + s[k].x + j] = main_bg[(i + s[k].y) * 256 + s[k].x + j];
		else
			for(i = 0; i < s[k].height; i++)
				for(j = s[k].width - offset; j < s[k].width && s[k].x + j < s[k].maxwidth; j++)
					backbuffer[(i + s[k].y) * 256 + s[k].x + j] = s[k].background[i * s[k].bg_width] | BIT(15);

	}

	/* the nice progressbar */
	if(screen_type == NOW_PLAYING) {
		float pos;

		for(i = 0; i < progressbar->ysize; i++)
			for(j = 0; j < progressbar->xsize; j++)
				backbuffer[(i + progressbar->ypos) * 256 + j + progressbar->xpos] = main_bg[(i + progressbar->ypos) * 256 + j + progressbar->xpos];

		for(i = 0; i < volumebar->ysize; i++)
			for(j = 0; j < volumebar->xsize; j++)
				backbuffer[(i + volumebar->ypos) * 256 + j + volumebar->xpos] = main_bg[(i + volumebar->ypos) * 256 + j + volumebar->xpos];

		for(i = 0; i < seekbar->ysize; i++)
			for(j = 0; j < seekbar->xsize; j++)
				backbuffer[(i + seekbar->ypos) * 256 + j + seekbar->xpos] = main_bg[(i + seekbar->ypos) * 256 + j + seekbar->xpos];

		if((nowplaying_mode == NOWPLAYING_SEEK && nowplaying_delay == 0)
			|| nowplaying_mode == NOWPLAYING_VOLUME) {

			nowplaying_mode = NOWPLAYING_VOLUME;

			if(state == PLAYING)
				progress_offset++;

			pos = ((float) sound_position() * (float) (progressbar->xsize)) / (float) sound_size();
			volumepic.attribute[0] = ATTR0_DISABLED;
			seekpospic.attribute[0] = ATTR0_DISABLED;
			volumepospic.attribute[0] = ATTR0_DISABLED;
			if(progressbar->type == 0) {
				progresspospic.attribute[0] = ATTR0_DISABLED;
			} else if(progressbar->type == 1) {
				progresspospic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (progressbar->seekpos_ypos);
				progresspospic.attribute[1] &= ~511;
				progresspospic.attribute[1] |= (progressbar->xpos - progressbar->seekpos_size / 2 + (int) pos);
			}

			if(progressbar->type == 0) {
				if(progressbar->full_animate == 1) {
					for(i = 0; i < progressbar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + progressbar->ypos) * 256 + j + progressbar->xpos] = progressbar->full[i * progressbar->full_size + (j + progress_offset) % progressbar->full_size];
				} else {
					for(i = 0; i < progressbar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + progressbar->ypos) * 256 + j + progressbar->xpos] = progressbar->full[i * progressbar->full_size + j % progressbar->full_size];
				}

				for(i = 0; i < progressbar->ysize; i++)
					for(j = pos; j < progressbar->xsize; j++)
						backbuffer[(i + progressbar->ypos) * 256 + j + progressbar->xpos] = progressbar->empty[i * progressbar->empty_size + j % progressbar->empty_size];
			} else if(progressbar->type == 1) {
				for(i = 0; i < progressbar->ysize; i++)
					for(j = 0; j < progressbar->xsize; j++)
						backbuffer[(i + progressbar->ypos) * 256 + j + progressbar->xpos] = progressbar->seekbar[i * progressbar->seekbar_size + j % progressbar->seekbar_size];
			}


		} else if(nowplaying_mode == NOWPLAYING_VOLUME_CHANGING) {
			progress_offset++;

			if(nowplaying_delay > 0)
				nowplaying_delay--;
			else
				nowplaying_mode = NOWPLAYING_VOLUME;

			pos = ((float) IPC2->sound_volume * (float) (volumebar->xsize)) / (float) 0xffff;
			volumepic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (volume_ypos);
			progresspospic.attribute[0] = ATTR0_DISABLED;
			seekpospic.attribute[0] = ATTR0_DISABLED;
			if(volumebar->type == 0) {
				volumepospic.attribute[0] = ATTR0_DISABLED;
			} else if(volumebar->type == 1) {
				volumepospic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (volumebar->seekpos_ypos);
				volumepospic.attribute[1] &= ~511;
				volumepospic.attribute[1] |= (volumebar->xpos - volumebar->seekpos_size / 2 + (int) pos);
			}

			if(volumebar->type == 0) {
				if(volumebar->full_animate == 1) {
					for(i = 0; i < volumebar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + volumebar->ypos) * 256 + j + volumebar->xpos] = volumebar->full[i * volumebar->full_size + (j + progress_offset) % volumebar->full_size];
				} else {
					for(i = 0; i < volumebar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + volumebar->ypos) * 256 + j + volumebar->xpos] = volumebar->full[i * volumebar->full_size + j % volumebar->full_size];
				}

				for(i = 0; i < volumebar->ysize; i++)
					for(j = pos; j < volumebar->xsize; j++)
						backbuffer[(i + volumebar->ypos) * 256 + j + volumebar->xpos] = volumebar->empty[i * volumebar->empty_size + j % volumebar->empty_size];
			} else if(volumebar->type == 1) {
				for(i = 0; i < volumebar->ysize; i++)
					for(j = 0; j < volumebar->xsize; j++)
						backbuffer[(i + volumebar->ypos) * 256 + j + volumebar->xpos] = volumebar->seekbar[i * volumebar->seekbar_size + j % volumebar->seekbar_size];
			}


		} else if(nowplaying_mode == NOWPLAYING_SEEK && nowplaying_delay > 0) {
			nowplaying_delay--;

			if(state == PLAYING)
				progress_offset++;

			pos = ((float) sound_position() * (float) (seekbar->xsize)) / (float) sound_size();
			volumepic.attribute[0] = ATTR0_DISABLED;
			progresspospic.attribute[0] = ATTR0_DISABLED;
			volumepospic.attribute[0] = ATTR0_DISABLED;
			if(seekbar->type == 0) {
				seekpospic.attribute[0] = ATTR0_DISABLED;
			} else if(seekbar->type == 1) {
				seekpospic.attribute[0] = ATTR0_BMP | ATTR0_COLOR_16 | (seekbar->seekpos_ypos);
				seekpospic.attribute[1] &= ~511;
				seekpospic.attribute[1] |= (seekbar->xpos - seekbar->seekpos_size / 2 + (int) pos);
			}

			if(seekbar->type == 0) {
				if(seekbar->full_animate == 1) {
					for(i = 0; i < seekbar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + seekbar->ypos) * 256 + j + seekbar->xpos] = seekbar->full[i * seekbar->full_size + (j + progress_offset) % seekbar->full_size];
				} else {
					for(i = 0; i < seekbar->ysize; i++)
						for(j = 0; j < pos; j++)
							backbuffer[(i + seekbar->ypos) * 256 + j + seekbar->xpos] = seekbar->full[i * seekbar->full_size + j % seekbar->full_size];
				}

				for(i = 0; i < seekbar->ysize; i++)
					for(j = pos; j < seekbar->xsize; j++)
						backbuffer[(i + seekbar->ypos) * 256 + j + seekbar->xpos] = seekbar->empty[i * seekbar->empty_size + j % seekbar->empty_size];

			} else if(seekbar->type == 1) {

				for(i = 0; i < seekbar->ysize; i++)
					for(j = 0; j < seekbar->xsize; j++)
						backbuffer[(i + seekbar->ypos) * 256 + j + seekbar->xpos] = seekbar->seekbar[i * seekbar->seekbar_size + j % seekbar->seekbar_size];

			}
		}

		/* display current time */
		{
			static int oldsize = 0;
			char t[128];

			for(i = 0; i < 16; i++)
				for(j = 0; j < oldsize; j++)
					backbuffer[(i + currentskin.time_ypos) * 256 + currentskin.time_xpos + j] = main_bg[(i + currentskin.time_ypos) * 256 + currentskin.time_xpos + j];

			sprintf(t, "%d:%02d / %d:%02d",
				sound_elapsed / 60, sound_elapsed % 60,
				sound_time  / 60, sound_time % 60);

			oldsize = getStringWidth(t, font);

			dispString2(currentskin.time_xpos,
				currentskin.time_ypos,
				currentskin.time_mask,
				t, backbuffer, font,
				screen_width + screen_xpos - 40,
				screen_height + screen_ypos,
				256);
		}
	}

	screen_switchbuffers();
}

void screen_update(void) {
	static struct screen_menu_entry *lastmenu = NULL;
	static u32 lastselected = 0xffffffff;
	static enum screen_type lasttype = OTHER;

	screen_update_battery();
	screen_update_hold();
	screen_update_state();

	if(screen_type != lasttype) {
		IPC2->sound_control = IPC2_SOUND_CLICK;

		if(lasttype == NOW_PLAYING) {
			albumpic.attribute[0] = ATTR0_DISABLED;
			volumepic.attribute[0] = ATTR0_DISABLED;
			shufflepic.attribute[0] = ATTR0_DISABLED;
			seekpospic.attribute[0] = ATTR0_DISABLED;

			if(screen_scroll_title)
				screen_scroll_title->animate = 0;
			if(screen_scroll_artist)
				screen_scroll_artist->animate = 0;
			if(screen_scroll_album)
				screen_scroll_album->animate = 0;

			nowplaying_delay = 0;
		}

		if(lasttype == MENU) {
			screen_scrolling_text[3].animate = 0;
		}

		lasttype = screen_type;

		if(screen_type == NOW_PLAYING) {
			lastselected = 0xffffffff;

			screen_nowplaying_reset();
		}
	}

	if(lasttype == MENU) {
		if(lastmenu != currentmenu || lastselected != screen_menu_selected) {
			IPC2->sound_control = IPC2_SOUND_CLICK;

			screen_reset_backbuffer();
			screen_drawmenu(currentmenu);
			screen_reset_backbuffer();
			screen_drawmenu(currentmenu);

			lastmenu = currentmenu;
			lastselected = screen_menu_selected;
		}
	}

	screen_animate_update();

	input_handleinput();
}
