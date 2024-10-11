#ifndef _ST7789V_H
#define _ST7789V_H

#include "ameba_soc.h"

typedef struct {
	void (*VBlank)(void *user_data);
} ST7789VVBlankCallback;

void st7789v_init(void);
void st7789v_get_info(int *width, int *height);
void st7789v_clean_invalidate_buffer(u8 *buffer);

void st7789v_register_callback(ST7789VVBlankCallback *callback, void *data);

#endif