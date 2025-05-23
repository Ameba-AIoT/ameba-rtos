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
void lv_hal_init(void)
{
	lv_init();

	display_init();

	int width;
	int height;
	display_get_info(&width, &height);

	uint8_t *buffer_1 = display_get_buffer(0);
	uint8_t *buffer_2 = display_get_buffer(1);

	// /*Initialize a descriptor for the buffer*/
	static lv_disp_draw_buf_t disp_buf;
	lv_disp_draw_buf_init(&disp_buf, buffer_1, buffer_2, width * height);

	// /*Initialize and register a display driver*/
	static lv_disp_drv_t disp_drv;
	lv_disp_drv_init(&disp_drv);
	disp_drv.draw_buf   = &disp_buf;
	disp_drv.flush_cb   = display_flush;
	disp_drv.hor_res    = width;
	disp_drv.ver_res    = height;
	disp_drv.full_refresh = 1;
	lv_disp_drv_register(&disp_drv);

}


/*Set in lv_conf.h as `LV_TICK_CUSTOM_SYS_TIME_EXPR`*/
uint32_t custom_tick_get(void)
{
	return rtos_time_get_current_system_time_ms();
}
