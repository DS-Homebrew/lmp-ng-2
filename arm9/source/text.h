#include <nds.h>

u16 getStringWidth(const char *s, uint16 **f);
u16 getStringHeight(const char *s, uint16 **f);
void dispString(s16 x_offset, s16 y_offset, u16 mask, const char *text, u16 *buffer, uint16 **font, u16 width, u16 height, u16 surface_width);
void dispString2(s16 x_offset, s16 y_offset, u16 mask, const char *text, u16 *buffer, uint16 **font, u16 width, u16 height, u16 surface_width);
