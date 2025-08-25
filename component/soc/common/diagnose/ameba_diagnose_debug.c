/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_diagnose_types.h"
#include "ameba_diagnose.h"
#include "ameba_soc.h"
#include "os_wrapper.h"
#include <string.h>
#include <stdlib.h>

char *generate_random_string(int min_length, int max_length)
{
	static const char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int charset_length = sizeof(charset) - 1;

	// 确保最小长度不小于0，最大长度不小于最小长度
	if (min_length < 0) {
		min_length = 0;
	}
	if (max_length < min_length) {
		max_length = min_length;
	}

	// 生成随机长度
	int length = min_length + _rand() % (max_length - min_length + 1);

	// 分配内存
	char *random_string = rtos_mem_malloc(length + 1); // +1 for null terminator
	if (random_string == NULL) {
		return NULL; // 内存分配失败
	}

	// 生成随机字符串
	for (int i = 0; i < length; i++) {
		random_string[i] = charset[_rand() % charset_length];
	}

	// 添加字符串结束符
	random_string[length] = '\0';

	return random_string;
}

void test_rtk_diag_multi_task1(void *pv_parameters)
{
	(void)pv_parameters;
	while (1) {
		char *str = generate_random_string(10, 30);
		rtk_diag_req_add_event_demo1(1, str);
		rtos_mem_free(str);
		rtos_time_delay_ms(300 + _rand() % 100); //delay: 100 ~ 199
	}

	rtos_task_delete(NULL);
}

void test_rtk_diag_multi_task2(void *pv_parameters)
{
	(void)pv_parameters;
	while (1) {
		char *str = generate_random_string(10, 30);
		rtk_diag_req_add_event_demo1(1, str);
		rtos_mem_free(str);
		rtos_time_delay_ms(400 + _rand() % 200); //delay: 100 ~ 199
	}

	rtos_task_delete(NULL);
}

void rtk_diag_debug_create_task(void)
{
	if (rtos_task_create(NULL, "diag_debug_task1", (rtos_task_t)test_rtk_diag_multi_task1, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGA("DIAG", "Cannot create power_ppe demo task\n");
	}
	if (rtos_task_create(NULL, "diag_debug_task2", (rtos_task_t)test_rtk_diag_multi_task2, NULL, (3072), (1)) != RTK_SUCCESS) {
		RTK_LOGA("DIAG", "Cannot create power_ppe demo task\n");
	}
}