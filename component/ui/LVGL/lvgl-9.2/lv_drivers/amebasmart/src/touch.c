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

#include "cst328.h"
#include "lvgl.h"

static uint16_t g_x = 0;
static uint16_t g_y = 0;
static uint8_t g_status = 3;

static void touch_data_callback(cst328_touch_data_t data)
{
	g_x = data.x;
	g_y = data.y;
	if (data.state == TOUCH_PRESS) {
		g_status = LV_INDEV_STATE_PR;
	} else {
		g_status = LV_INDEV_STATE_REL;
	}
}

void touch_init(void)
{
	cst328_init();
	cst328_register_touch_data_callback(touch_data_callback);
}

void touch_read(lv_indev_t * indev, lv_indev_data_t * data)
{
	(void)indev;
	data->point.x = g_x;
	data->point.y = g_y;
	data->state = g_status;
}