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

void vAssertCalled(const char *pcInfo, uint32_t ulLine);
void rtk_assert_printf(const char *fmt, ...);

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
