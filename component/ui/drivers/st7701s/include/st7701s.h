#ifndef _ST7701S_H
#define _ST7701S_H

#include "ameba_soc.h"

typedef enum {
	RGB565,
	ARGB8888,
} st7701s_image_format;

typedef struct {
	void (*VBlank)(void *user_data);
} ST7701SVBlankCallback;
void st7701s_init(int image_format);
void st7701s_get_info(int *width, int *height);
void st7701s_clean_invalidate_buffer(u8 *buffer);
void st7701s_register_callback(ST7701SVBlankCallback *callback, void *data);

#endif