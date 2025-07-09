#ifndef _ST7272A_H
#define _ST7272A_H

#include "ameba_soc.h"

typedef enum {
	RGB888,
	ARGB8888,
} st7272s_image_format;

typedef struct {
	void (*VBlank)(void *user_data);
} ST7272AVBlankCallback;
void st7272a_init(int image_format);
void st7272a_get_info(int *width, int *height);
void st7272a_clean_invalidate_buffer(u8 *buffer);
void st7272a_register_callback(ST7272AVBlankCallback *callback, void *data);

#endif
