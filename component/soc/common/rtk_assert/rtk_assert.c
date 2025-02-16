/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Copyright (c) 2023 Realtek Semiconductor Corp. All rights reserved.
 *
 * This software is a confidential and proprietary property of Realtek
 * Semiconductor Corp. Disclosure, reproduction, redistribution, in
 * whole or in part, of this work and its derivatives without express
 * permission is prohibited.
 *
 * Realtek Semiconductor Corp. reserves the right to update, modify, or
 * discontinue this software at any time without notice. This software is
 * provided "as is" and any express or implied warranties, including, but
 * not limited to, the implied warranties of merchantability and fitness for
 * a particular purpose are disclaimed. In no event shall Realtek
 * Semiconductor Corp. be liable for any direct, indirect, incidental,
 * special, exemplary, or consequential damages (including, but not limited
 * to, procurement of substitute goods or services; loss of use, data, or
 * profits; or business interruption) however caused and on any theory of
 * liability, whether in contract, strict liability, or tort (including
 * negligence or otherwise) arising in any way out of the use of this software,
 * even if advised of the possibility of such damage.
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
