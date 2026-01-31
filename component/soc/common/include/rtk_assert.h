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

void vAssertCalled(const char *pcFile, uint32_t ulLine);

#if defined(CONFIG_ASSERTION_DISABLE)
#define rtk_assert(__e) ((void)(__e))
#elif defined(CONFIG_ASSERTION_SILENT) && CONFIG_ASSERTION_SILENT
#define rtk_assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(NULL, 0))
#else
#define rtk_assert(__e) (__builtin_expect(!!(__e), 1) ? (void)0 : vAssertCalled(__FILE__, __LINE__))
#endif

#ifdef __cplusplus
}
#endif

#endif
