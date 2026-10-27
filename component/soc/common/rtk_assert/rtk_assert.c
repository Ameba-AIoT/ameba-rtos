/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "ameba_soc.h"
#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
#include "os_wrapper.h"
#include "unity.h"

void *volatile g_unity_abort_task = NULL;
#endif

void __rtos_critical_enter_os(void);
void __rtos_critical_exit_os(void);

extern void LOGUART_PutChar(u8 c);

/* Print val in decimal without leading zeros.
 * Out of line on purpose: inlined, its registers would be saved in the caller's frame. */
__NO_INLINE
static void print_dec_value(uint32_t val)
{
	uint32_t div = 1;

	/* div = largest power of 10 that is not greater than val */
	while (val / div >= 10) {
		div *= 10;
	}

	/* print one digit per power of 10, most significant first */
	do {
		LOGUART_PutChar('0' + val / div);
		val %= div;
		div /= 10;
	} while (div != 0);
}

static void print_str(const char *s)
{
	while (s != NULL && *s != '\0') {
		LOGUART_PutChar(*s++);
	}
}

/* Prints "Assert Error! <info>:<line>\r\n".
 * A macro, not a function: an extra call level would add a stack frame to the assert path. */
#define PRINT_ASSERT_MSG(info, line) do { \
		print_str("Assert Error! "); \
		print_str(info); \
		print_str(":"); \
		print_dec_value(line); \
		print_str("\r\n"); \
	} while (0)

__NO_RETURN _OPTIMIZE_NONE_
void vAssertCalled(const char *pcInfo, uint32_t ulLine)
{
	__rtos_critical_enter_os();
	PRINT_ASSERT_MSG(pcInfo, ulLine);

#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
	/* longjmp only if the current task owns the live setjmp frame */
	if (g_unity_abort_task != NULL && g_unity_abort_task == rtos_task_handle_get() && !rtos_critical_is_in_interrupt()) {
		__rtos_critical_exit_os();
		TEST_ABORT();
	}
#endif
	for (;;);
}
