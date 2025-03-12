/**
 ******************************************************************************
 * @file    main.c
 * @brief   Main file
 *          Load the app and also turn on a LED
 ******************************************************************************
 * @attention
 *
 * LICENSE
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2020 Rohit Gujarathi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/

#include "ameba.h"
#include "os_wrapper.h"
#include "syscall.h"
#include "app_loader.h"
#include "vfs.h"
#include "app_ioctrl.h"

static const char *TAG = "APP_LOADER";

static void example_main_task(void *param)
{
	UNUSED(param);
	apps_1_struct_t *apps_1_info = (apps_1_struct_t *)malloc(sizeof(apps_1_struct_t));
	apps_2_struct_t *apps_2_info = (apps_2_struct_t *)malloc(sizeof(apps_2_struct_t));

	if (apps_1_info) {
		if (APP_OK != LoadAppWithName("target_apps_1", apps_1_info, sizeof(apps_1_struct_t))) {
			free(apps_1_info);
			apps_1_info = NULL;
		}
	}

	if (apps_2_info) {
		if (APP_OK != LoadAppWithName("target_apps_2", apps_2_info, sizeof(apps_2_struct_t))) {
			free(apps_2_info);
			apps_2_info = NULL;
		}
	}

	RTK_LOGI(TAG, "------------->end\n");

	if (apps_1_info) {
		uint32_t arg = 2;
		RTK_LOGI(TAG, "------------->first call\n");
		app_ioctrl(apps_1_info, MOD_APP_CMD_PASS_PARAMS, (void *)arg);
		RTK_LOGI(TAG, "------------->second call\n");
		app_ioctrl(apps_1_info, MOD_APP_CMD_PASS_PARAMS, (void *)arg);
		app_ioctrl(apps_1_info, MOD_APP_CMD_TEST_SOCKET, NULL);
		app_ioctrl(apps_1_info, MOD_APP_CMD_APP_MAIN, NULL);
	}

	if (apps_2_info) {
		app_ioctrl(apps_2_info, MOD_APP_CMD_TEST_PTHREAD, NULL);
	}

	rtos_task_delete(NULL);
}

void test_app_example(void)
{
	if (rtos_task_create(NULL, ((const char *)"example_main_task"), example_main_task, NULL, 4096 * 4, 1) != SUCCESS) {
		RTK_LOGE(TAG, "Failed to create example_main_task\n\r");
	}
}

void PICTestApp(u8  *argv[])
{
	if (_strcmp((const char *)argv[0], "load") == 0) {
		test_app_example();
	}
}

u32
CmdPICTest(
	IN u16 argc,
	IN u8  *argv[]
)
{
	if (argc > 0) {
		PICTestApp(argv);
	}

	return TRUE;
}

CMD_TABLE_DATA_SECTION
const COMMAND_TABLE pic_test_cmd_table[] = {
	{
		(const u8 *)"pic", 3, CmdPICTest, (const u8 *)"\tWIFI \n"
	},
};



