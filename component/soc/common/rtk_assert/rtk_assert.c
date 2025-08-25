/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "ameba_soc.h"

void __rtos_critical_enter_os(void);

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
	for (;;);
}
