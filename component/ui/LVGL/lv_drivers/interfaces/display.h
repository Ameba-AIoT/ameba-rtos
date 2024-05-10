#ifndef UI_LVGL_LV_DRIVERS_DISPLAY
#define UI_LVGL_LV_DRIVERS_DISPLAY

#include "lvgl.h"

void display_init(void);
void display_get_info(int *width, int *height);
uint8_t *display_get_buffer(int buffer_id);
void display_render_start(void);
void display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p);

#endif /* UI_LVGL_LV_DRIVERS_DISPLAY */
