/**
 ******************************************************************************
 * @file    app_ioctrl.c
 * @brief   Contains function for loading the app and setting up the freertos
 *          task
 ******************************************************************************
*/

#include "ameba.h"
#include "FreeRTOS.h"
#include "task.h"
#include <string.h> //TODO: Replace with a minimal libc implementation
#include <stdlib.h> //TODO: Replace with a minimal libc implementation
#include "app_ioctrl.h"
#include "syscall.h"

static const char *TAG = "APP_IOCTRL";

static int mod_test_1_call(void *mod, int cmd, void *args)
{
	apps_1_struct_t *apps = (apps_1_struct_t *)mod;
	apps->magic.set_got_base(apps->magic.got_base_addr);

	switch (cmd) {
	case MOD_APP_CMD_TEST_PTHREAD: {
		func0_t test_pthread = (func0_t)(uint32_t)(apps->test_pthread);
		RTK_LOGD(TAG, "Check entry point: 0x%08X Data: 0x%08lX\n", (uint32_t *)test_pthread, *(uint32_t *)test_pthread);
		RTK_LOGD(TAG, "Data at dummy_function entry point: 0x%08X\n", *((uint32_t *)(((uintptr_t)test_pthread) & 0xFFFFFFFE)));
		test_pthread();
	}
	break;
	case MOD_APP_CMD_TEST_SOCKET: {
		func0_t test_socket = (func0_t)(uint32_t)(apps->test_socket);
		RTK_LOGD(TAG, "Check entry point: 0x%08X Data: 0x%08lX\n", (uint32_t *)test_socket, *(uint32_t *)test_socket);
		RTK_LOGD(TAG, "Data at dummy_function entry point: 0x%08X\n", *((uint32_t *)(((uintptr_t)test_socket) & 0xFFFFFFFE)));
		test_socket();
	}
	break;
	case MOD_APP_CMD_PASS_PARAMS: {
		func1_t test_pass_params = (func1_t)(uint32_t)(apps->test_pass_params);
		test_pass_params(args);
	}
	break;
	case MOD_APP_CMD_APP_MAIN: {
		func0_t app_main = (func0_t)(apps->app_main);
		app_main();
	}
	break;
	default:
		RTK_LOGI(TAG, "Not supported! cmd: %d\n", cmd);
		return 1;
	}
	return 0;
}

static int mod_test_2_call(void *mod, int cmd, void *args)
{
	(void)args;
	apps_2_struct_t *apps = (apps_2_struct_t *)mod;
	apps->magic.set_got_base(apps->magic.got_base_addr);

	switch (cmd) {
	case MOD_APP_CMD_TEST_PTHREAD: {
		apps->test_pthread();
	}
	break;
	default:
		RTK_LOGI(TAG, "Not supported! cmd: %d\n", cmd);
		return 1;
	}
	return 0;
}


int app_ioctrl(void *mod, int cmd, void *args)
{
	uint8_t magic[8];
	memcpy(magic, mod, 8);

	asm volatile("MOV %0, r9\n"	:"=r"(reg_value)::"memory");
	RTK_LOGI(TAG, "reg_value: %x\n", reg_value);

	if (strncmp((const char *)magic, "appinfo", 8) == 0) {
		RTK_LOGI(TAG, "module magic: %s\n", magic);

		apps_common_magic_t *common_magic = (apps_common_magic_t *)mod;

		RTK_LOGI(TAG, "module type: %x\n", common_magic->type);

		switch (common_magic->type) {
		case AMOD_TYPE_MOD_TEST_1:
			mod_test_1_call(mod, cmd, args);
			break;
		case AMOD_TYPE_MOD_TEST_2:
			mod_test_2_call(mod, cmd, args);
			break;
		}
	} else {
		RTK_LOGI(TAG, "module magic: %s\n", magic);
		asm volatile("MOV r9, %0\n"	::"r"(reg_value):"memory");
		return -1;
	}
	asm volatile("MOV r9, %0\n"	::"r"(reg_value):"memory");
	return 0;
}
