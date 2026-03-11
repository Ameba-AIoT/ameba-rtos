/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "ameba_soc.h"
#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
#include "unity.h"
#endif

void __rtos_critical_enter_os(void);
void __rtos_critical_exit_os(void);

void vAssertCalled(const char *pcFile, uint32_t ulLine)
{
	UNUSED(pcFile);
	UNUSED(ulLine);
	__rtos_critical_enter_os();
#if defined(CONFIG_ASSERTION_ENABLE)
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Assert Error! %s:%u\r\n", pcFile, ulLine);
#elif defined(CONFIG_ASSERTION_SILENT) && CONFIG_ASSERTION_SILENT
	RTK_LOGS(NOTAG, RTK_LOG_ERROR, "Assert Error!\r\n");
#endif

#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
	__rtos_critical_exit_os();
	TEST_ABORT();
#else
	for (;;);
#endif
}
