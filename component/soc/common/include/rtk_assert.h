/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RTK_ASSERT_
#define __RTK_ASSERT_

#ifdef __cplusplus
extern "C" {
#endif

__attribute__((__noreturn__)) void vAssertCalled(const char *pcInfo, uint32_t ulLine);

/* Straight to LOGUART_PutChar: no lock, no engine, a small stack. For paths where nothing else can be relied on (assert, crash dump). */
void rtk_assert_print_str(const char *s);
void rtk_assert_print_dec(uint32_t val);

#if (defined(CONFIG_WHC_HOST) || defined(CONFIG_WHC_NONE)) && defined(CONFIG_UNITY_TEST)
/* Task owning the live Unity setjmp frame (TEST_PROTECT window); NULL when none. */
extern void *volatile g_unity_abort_task;
#endif

/*
 * rtk_assert(__e, _comp):
 *   __e    - expression that must hold true
 *   _comp  - short component tag string (e.g. "lwip", "freertos")
 *
 * Selected via the "RTK Assert Level" Kconfig choice:
 *   CONFIG_ASSERTION_DISABLE : assertion compiled out (no-op).
 *   CONFIG_ASSERTION_COMP    : (default) on failure print the component tag and
 *                              line (avoids embedding the full __FILE__ path string).
 *   CONFIG_ASSERTION_DETAIL  : on failure print the file and line.
 */
#if defined(CONFIG_ASSERTION_DISABLE)
#define rtk_assert(__e, _comp) ((void)(__e))
#elif defined(CONFIG_ASSERTION_COMP) && CONFIG_ASSERTION_COMP
#define rtk_assert(__e, _comp) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(_comp, __LINE__))
#else /* CONFIG_ASSERTION_DETAIL */
#define rtk_assert(__e, _comp) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(__FILE__, __LINE__))
#endif

#ifdef __cplusplus
}
#endif

#endif
