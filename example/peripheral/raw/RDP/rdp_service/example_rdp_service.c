/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ameba_soc.h"
#include "os_wrapper.h"
#include <stdio.h>
#include "rdp_service_example.h"

static const char *const TAG = "RDP_SERVICE";

u32 no_protection_func(u32 data)
{
	u32 result;

	result = data / 5 + 3;
	result *= 2;
	result += 8;

	return result;
}

void rdp_service_demo(void)
{
	/* Wait for other CPU boot done */
	rtos_time_delay_ms(1000);

	RTK_LOGI(TAG, "=== RDP Service Demo Start ===\n");

	if (SYSCFG_OTP_RDPEn() == FALSE) {
		RTK_LOGE(TAG, "RDP bit In OTP is not enabled!\n");
		goto end;
	}

	int i = 0;
	u32 rdp_result;
	u32 no_rdp_result;

	/* Tasks are not created with a secure context. Any task that is going to call secure functions must call
		rtos_create_secure_context() to allocate itself a secure context before it calls any secure function. */
	rtos_create_secure_context(1024);

	for (i = 0; i < 32; i++) {
		rdp_result = rdp_protection_entry(i);
		no_rdp_result = rdp_no_protection_call(no_protection_func, i);

		if (rdp_result != no_rdp_result) {
			RTK_LOGE(TAG, "RDP call fail at index %d!\n", i);
			RTK_LOGE(TAG, "rdp_result = 0x%x, no_rdp_result = 0x%x\n",
					 (unsigned int)rdp_result, (unsigned int)no_rdp_result);
			goto end;
		}
	}

	RTK_LOGI(TAG, "=== RDP Service Demo Complete ===\n");
	RTK_LOGI(TAG, "All 32 tests passed! RDP call succeed!\n");

end:
	rtos_task_delete(NULL);
}

void example_rdp_service(void)
{
#ifdef CONFIG_TRUSTZONE_EN
	RTK_LOGI(TAG, "RDP Service Demo main\n");

	/* Create demo task */
	if (rtos_task_create(NULL, "RDP_SERVICE_DEMO", (rtos_task_t)rdp_service_demo, NULL, 4096, (1)) != RTK_SUCCESS) {
		RTK_LOGE(TAG, "Cannot create demo task\n");
		while (1);
	}
#else
	RTK_LOGE(TAG, "Please enable TrustZone in menuconfig\n");
#endif
}
