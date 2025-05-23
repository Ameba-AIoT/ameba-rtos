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

#include <sys/time.h>
#include <stdio.h>

#include "lvgl.h"
#include "display.h"

#include "lv_hal.h"
#include "os_wrapper.h"

uint32_t custom_tick_get(void)
{
	return rtos_time_get_current_system_time_ms();
}

void lv_hal_init(void)
{
	lv_init();

	lv_tick_set_cb(custom_tick_get);

	display_init();

	int width;
	int height;
	display_get_info(&width, &height);

	uint8_t *buffer_1 = display_get_buffer(0);
	uint8_t *buffer_2 = display_get_buffer(1);

	lv_display_t *display = lv_display_create(width, height); /*Create the display*/
	lv_display_set_flush_cb(display, display_flush);        /*Set a flush callback to draw to the display*/
	lv_display_set_buffers(display, buffer_1, buffer_2, width * height * LV_COLOR_DEPTH, LV_DISPLAY_RENDER_MODE_FULL); /*Set an initialized buffer*/
	//lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
}
