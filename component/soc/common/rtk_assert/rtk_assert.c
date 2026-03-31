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

/* Forward declaration - use ROM function when available */
extern void LOGUART_PutChar(u8 c);

/* Helper: convert hex to ASCII */
static inline char hex_digit(u8 nibble)
{
	if (nibble < 10) {
		return '0' + nibble;
	} else {
		return 'a' + (nibble - 10);
	}
}

/* Helper: print hex value (ptr) */
static void print_hex_value(uintptr_t val)
{
	int i;
	for (i = (sizeof(uintptr_t) * 8) - 4; i >= 0; i -= 4) {
		LOGUART_PutChar(hex_digit((val >> i) & 0xF));
	}
}

/* Helper: print decimal value */
static void print_dec_value(uint32_t val)
{
	char buf[16];
	int idx = sizeof(buf) - 1;

	buf[idx] = '\0';
	idx--;

	if (val == 0) {
		buf[idx--] = '0';
	} else {
		while (val > 0) {
			buf[idx--] = '0' + (val % 10);
			val /= 10;
		}
	}
	idx++;

	while (buf[idx] != '\0') {
		LOGUART_PutChar(buf[idx++]);
	}
}

/* Lightweight printf - only supports %s (string), %p (ptr) and %u (unsigned) */
static void rtk_assert_printf(const char *fmt, const char *str, uintptr_t ptr, uint32_t num)
{
	const char *p = fmt;
	while (*p != '\0') {
		if (*p == '%') {
			p++;
			if (*p == 's' && str != NULL) {
				while (*str != '\0') {
					LOGUART_PutChar(*str++);
				}
				p++;
			} else if (*p == 'p') {
				print_hex_value(ptr);
				p++;
			} else if (*p == 'u') {
				print_dec_value(num);
				p++;
			} else {
				/* Unknown format, just output % and continue */
				LOGUART_PutChar('%');
			}
		} else {
			LOGUART_PutChar(*p++);
		}
	}
}

__attribute__((optimize("O0")))
void vAssertCalled(const char *pcFile, uint32_t ulLine)
{
	__rtos_critical_enter_os();
#if defined(CONFIG_ASSERTION_ENABLE)
	rtk_assert_printf("Assert Error! %s:%u\r\n", pcFile, 0, ulLine);
#elif defined(CONFIG_ASSERTION_SILENT) && CONFIG_ASSERTION_SILENT
	rtk_assert_printf("Assert Error!\r\n", NULL, 0, 0);
#endif

#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
	__rtos_critical_exit_os();
	TEST_ABORT();
#else
	for (;;);
#endif
}
