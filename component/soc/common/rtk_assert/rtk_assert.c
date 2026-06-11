/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "stdio.h"
#include "stdarg.h"
#include "ameba_soc.h"
#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
#include "unity.h"
#endif

void __rtos_critical_enter_os(void);
void __rtos_critical_exit_os(void);

extern void LOGUART_PutChar(u8 c);

static inline char hex_digit(u8 nibble)
{
	if (nibble < 10) {
		return '0' + nibble;
	} else {
		return 'a' + (nibble - 10);
	}
}

static void print_hex_value(uintptr_t val)
{
	int i;
	for (i = (sizeof(uintptr_t) * 8) - 4; i >= 0; i -= 4) {
		LOGUART_PutChar(hex_digit((val >> i) & 0xF));
	}
}

static void print_hex32_value(uint32_t val)
{
	int i;
	for (i = (sizeof(uint32_t) * 8) - 4; i >= 0; i -= 4) {
		LOGUART_PutChar(hex_digit((val >> i) & 0xF));
	}
}

/* uint32_t max (4294967295) is 10 digits; buf[11] = 10 digits + null */
static void print_dec_value(uint32_t val)
{
	char buf[11];
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

/* Lightweight printf - supports %s (string), %p (ptr), %u (unsigned), %x (hex) */
void rtk_assert_printf(const char *fmt, ...)
{
	va_list ap;
	const char *p = fmt;
	va_start(ap, fmt);
	while (*p != '\0') {
		if (*p == '%') {
			p++;
			if (*p == 's') {
				const char *s = va_arg(ap, const char *);
				if (s) {
					while (*s != '\0') {
						LOGUART_PutChar(*s++);
					}
				}
				p++;
			} else if (*p == 'p') {
				print_hex_value((uintptr_t)va_arg(ap, void *));
				p++;
			} else if (*p == 'x') {
				print_hex32_value(va_arg(ap, unsigned int));
				p++;
			} else if (*p == 'u') {
				print_dec_value(va_arg(ap, unsigned int));
				p++;
			} else {
				/* Unknown format: emit % and let the loop re-read the
				 * specifier character as a literal on the next iteration */
				LOGUART_PutChar('%');
			}
		} else {
			LOGUART_PutChar(*p++);
		}
	}
	va_end(ap);
}

__attribute__((optimize("O0")))
void vAssertCalled(const char *pcInfo, uint32_t ulLine)
{
	__rtos_critical_enter_os();
	rtk_assert_printf("Assert Error! %s:%u\r\n", pcInfo, ulLine);

#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
	__rtos_critical_exit_os();
	TEST_ABORT();
#else
	for (;;);
#endif
}
