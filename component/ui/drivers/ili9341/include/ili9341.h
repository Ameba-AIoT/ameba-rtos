#ifndef _ILI9341_H
#define _ILI9341_H

#include "ameba_soc.h"

typedef struct {
	void (*VBlank)(void *user_data);
} ILI9341VBlankCallback;

void ili9341_init(void);
void ili9341_get_info(int *width, int *height);
void ili9341_clean_invalidate_buffer(u8 *buffer);

void ili9341_register_callback(ILI9341VBlankCallback *callback, void *data);

#endif
