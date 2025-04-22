/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "lvgl.h"
#include "ameba_soc.h"
#include "display.h"
#include "os_wrapper.h"

#include "st7701s.h"

static int g_width = 0;
static int g_height = 0;

static uint8_t *g_buffer_0 = NULL;
static uint8_t *g_buffer_1 = NULL;

static rtos_sema_t g_vsync_sem;
static int g_id = 0;
volatile int wait_for_vsync = 0;

void display_vsync_handle(void *data)
{
	UNUSED(data);
	if (wait_for_vsync == 1) {
		rtos_sema_give(g_vsync_sem);
	}
}

void display_init(void)
{
	int image_format = 0;
	if (LV_COLOR_DEPTH == 16) {
		image_format = RGB565;
	} else {
		image_format = ARGB8888;
	}
	st7701s_init(image_format);

	st7701s_get_info(&g_width, &g_height);

	g_buffer_0 = (uint8_t *)malloc(g_width * g_height * LV_COLOR_DEPTH / 8);
	g_buffer_1 = (uint8_t *)malloc(g_width * g_height * LV_COLOR_DEPTH / 8);

	rtos_sema_create(&g_vsync_sem, 0, RTOS_SEMA_MAX_COUNT);

	st7701s_clean_invalidate_buffer(g_buffer_1);

	ST7701SVBlankCallback *callback = (ST7701SVBlankCallback *)malloc(sizeof(ST7701SVBlankCallback));
	callback->VBlank = display_vsync_handle;
	st7701s_register_callback(callback, NULL);
}

void display_get_info(int *width, int *height)
{
	*width = g_width;
	*height = g_height;
}

uint8_t *display_get_buffer(int buffer_id)
{
	if (buffer_id == 1) {
		return g_buffer_1;
	} else if (buffer_id == 0) {
		return g_buffer_0;
	} else {
		return NULL;
	}
}

void display_flush(lv_disp_drv_t *disp_drv, const lv_area_t *area, lv_color_t *color_p)
{
	//printf("display_flush %d %d %d %d\n", area->x1, area->y1, area->x2, area->y2);
	UNUSED(area);
	UNUSED(color_p);

	if (g_id == 1) {
		st7701s_clean_invalidate_buffer(g_buffer_1);
		wait_for_vsync = 1;
		rtos_sema_take(g_vsync_sem, RTOS_MAX_TIMEOUT);
		wait_for_vsync = 0;
		g_id = 0;
	} else {
		st7701s_clean_invalidate_buffer(g_buffer_0);
		wait_for_vsync = 1;
		rtos_sema_take(g_vsync_sem, RTOS_MAX_TIMEOUT);
		wait_for_vsync = 0;
		g_id = 1;
	}

	lv_disp_flush_ready(disp_drv);
}