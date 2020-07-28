#include <nds.h>

#include <string.h>

#include <id3tag.h>
#include <tremor/ivorbiscodec.h>
#include <tremor/ivorbisfile.h>
#include <FLAC/metadata.h>

#include "file.h"

void tags_scan(struct media *m) {
	if(m->format == MAD) {
		struct id3_file *file;
		struct id3_tag *tag;

		unsigned int i;
		struct id3_frame const *frame;
		id3_ucs4_t const *ucs4;
		id3_latin1_t *latin1;

		static struct {
			char const *id;
			char const *label;
		} const info[] = {
			{ ID3_FRAME_TITLE,  "Title"         },
			{ ID3_FRAME_ARTIST, "Artist"        },
			{ ID3_FRAME_ALBUM,  "Album"         },
			//		{ ID3_FRAME_TRACK,  "Track"         },
			//		{ ID3_FRAME_YEAR,   "Year"          },
			//		{ ID3_FRAME_GENRE,  "Genre"         },
		};

		file = id3_file_open(m->path, ID3_FILE_MODE_READONLY);
		if(file == 0) {
			return;
		}

		tag = id3_file_tag(file);
		if(tag == 0)
			goto close;

		/* text information */

		for(i = 0; i < sizeof(info) / sizeof(info[0]); ++i) {
			union id3_field const *field;
			unsigned int nstrings, j;

			frame = id3_tag_findframe(tag, info[i].id, 0);
			if(frame == 0)
				continue;

			field    = id3_frame_field(frame, 1);
			nstrings = id3_field_getnstrings(field);

			for(j = 0; j < nstrings; ++j) {
				ucs4 = id3_field_getstrings(field, j);
				if(ucs4 == 0)
					goto close;

				if(strcmp(info[i].id, ID3_FRAME_GENRE) == 0)
					ucs4 = id3_genre_name(ucs4);

				latin1 = id3_ucs4_latin1duplicate(ucs4);
				if (latin1 == 0)
					goto close;

				if(j == 0 && info[i].label) {
					/* info[i].label = latin1 */

					if(strcmp(info[i].id, ID3_FRAME_TITLE) == 0) {
						if(m->title != NULL)
							free(m->title);

						m->title = strdup(latin1);
						if(m->title == NULL) {
							printf("tags_scan: malloc failed!\n");
							while(1)
								swiWaitForVBlank();
						}
					} else if(strcmp(info[i].id, ID3_FRAME_ARTIST) == 0) {
						if(m->artist != NULL)
							free(m->artist);

								m->artist = strdup(latin1);
						if(m->artist == NULL) {
							printf("tags_scan: malloc failed!\n");
							while(1)
								swiWaitForVBlank();
						}
					} else if(strcmp(info[i].id, ID3_FRAME_ALBUM) == 0) {
						if(m->album != NULL)
							free(m->album);

						m->album = strdup(latin1);
						if(m->album == NULL) {
							printf("tags_scan: malloc failed!\n");
							while(1)
								swiWaitForVBlank();
						}
					}
				}
				//			else {
				//				if(strcmp(info[i].id, "TCOP") == 0 || strcmp(info[i].id, "TPRO") == 0) {
				//					/* (info[i].id[1] == 'C') ? "Copyright (C)" : "Produced (P)" = latin1 */
				//				}
				//			}

				free(latin1);
			}
		}

		//	/* comments */
		//
		//	i = 0;
		//	while ((frame = id3_tag_findframe(tag, ID3_FRAME_COMMENT, i++))) {
		//		ucs4 = id3_field_getstring(id3_frame_field(frame, 2));
		//		if(ucs4 == 0)
		//			goto close;
		//
		//		if (*ucs4)
		//			continue;
		//
		//		ucs4 = id3_field_getfullstring(id3_frame_field(frame, 3));
		//		if(ucs4 == 0)
		//			goto close;
		//
		//		latin1 = id3_ucs4_latin1duplicate(ucs4);
		//		if (latin1 == 0)
		//			goto close;
		//
		//		/* comment = latin1 */
		//
		//		free(latin1);
		//		break;
		//	}

close:

		id3_file_close(file);
	}

	if(m->format == TREMOR) {
		FILE *fp;
		char *str = NULL;
		OggVorbis_File vf;
		vorbis_comment *vc;

		fp = fopen(m->path, "r");
		ov_open(fp, &vf, NULL, 0);

		vc = ov_comment(&vf, -1);

		str = vorbis_comment_query(vc, "TITLE", 0);
		if(str != NULL) {
			if(m->title != NULL)
				free(m->title);

			m->title = strdup(str);
			if(m->title == NULL) {
				printf("tags_scan: malloc failed!\n");
				while(1)
					swiWaitForVBlank();
			}
		}

		str = vorbis_comment_query(vc, "ARTIST", 0);
		if(str != NULL) {
			if(m->artist != NULL)
				free(m->artist);

			m->artist = strdup(str);
			if(m->title == NULL) {
				printf("tags_scan: malloc failed!\n");
				while(1)
					swiWaitForVBlank();
			}
		}

		str = vorbis_comment_query(vc, "YEAR", 0);
		if(str != NULL) {
			if(m->album != NULL)
				free(m->album);

			m->album = strdup(str);
			if(m->title == NULL) {
				printf("tags_scan: malloc failed!\n");
				while(1)
					swiWaitForVBlank();
			}
		}

		ov_clear(&vf);
	}

	if(m->format == FLAC) {
		FLAC__StreamMetadata *tags;
		FLAC__StreamMetadata_VorbisComment comment;
		int t;
		char *name, *field;

		FLAC__metadata_get_tags(m->path, &tags);

		t = FLAC__metadata_object_vorbiscomment_find_entry_from(tags, 0, "TITLE");
		if(t != -1) {
			comment = tags->data.vorbis_comment;
			FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair(comment.comments[t], &name, &field);

			if(m->title != NULL)
				free(m->title);
			m->title = field;
			free(name);
		}

		t = FLAC__metadata_object_vorbiscomment_find_entry_from(tags, 0, "ARTIST");
		if(t != -1) {
			comment = tags->data.vorbis_comment;
			FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair(comment.comments[t], &name, &field);

			if(m->artist != NULL)
				free(m->artist);
			m->artist = field;
			free(name);
		}

		t = FLAC__metadata_object_vorbiscomment_find_entry_from(tags, 0, "ALBUM");
		if(t != -1) {
			comment = tags->data.vorbis_comment;
			FLAC__metadata_object_vorbiscomment_entry_to_name_value_pair(comment.comments[t], &name, &field);

			if(m->album != NULL)
				free(m->album);
			m->album = field;
			free(name);
		}

		FLAC__metadata_object_delete(tags);
	}
}
