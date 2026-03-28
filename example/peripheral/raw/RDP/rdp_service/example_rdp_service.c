/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include "rdp_service_example.h"

static const char *const TAG = "RDP";

/**
 * Example callback: Process data in non-secure world.
 * Implement your own algorithm here.
 */
static uint32_t my_callback(uint32_t data)
{
	return (data ^ 0xDEADBEEF) + 0x12345678;
}

static void rdp_demo(void)
{
	uint32_t result_secure, result_callback;
	int i;

	rtos_time_delay_ms(1000);
	rtos_create_secure_context(1024);

	RTK_LOGI(TAG, "=== RDP Service Demo ===\n");

	/* Initialize RDP service with callback */
	rdp_service_init(my_callback);

	for (i = 0; i < 4; i++) {
		/* Method 1: Compute in secure world */
		result_secure = rdp_secure_compute(i);

		/* Method 2: Get data and compute via registered callback */
		result_callback = rdp_callback_process(i);

		RTK_LOGI(TAG, "idx=%d: secure=0x%08X, callback=0x%08X %s\n",
				 i, result_secure, result_callback,
				 (result_secure == result_callback) ? "OK" : "FAIL");
	}

	RTK_LOGI(TAG, "Demo complete!\n");
	rtos_task_delete(NULL);
}

void example_rdp_service(void)
{
#ifdef CONFIG_TRUSTZONE_EN
	if (rtos_task_create(NULL, "rdp_demo", (rtos_task_t)rdp_demo, NULL, 4096, 1) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Create task failed\n");
	}
#else
	RTK_LOGE(TAG, "Enable CONFIG_TRUSTZONE_EN in menuconfig first\n");
#endif
}
