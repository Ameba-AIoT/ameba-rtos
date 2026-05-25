/*
 * Copyright (c) 2011-2014 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef ZEPHYR_INCLUDE_SYS___ASSERT_H_
#define ZEPHYR_INCLUDE_SYS___ASSERT_H_

#include <stdbool.h>
#include <stdio.h>
#include <zephyr/toolchain.h>
#include <osif.h>
#include <zephyr_log.h>

#define __ASSERT_PRINT          z_printf
#define __ASSERT_POST_ACTION()  { }
#define __ASSERT_UNREACHABLE    do { osif_sched_stop(); for(;;);} while(0)
/* define k_oops to fix compile error, zephyr_porting */
#define k_oops __ASSERT_UNREACHABLE


#ifdef CONFIG_ASSERT_NO_MSG_INFO
#define __ASSERT_MSG_INFO(fmt, ...)
#else  /* CONFIG_ASSERT_NO_MSG_INFO */
#define __ASSERT_MSG_INFO(fmt, ...) __ASSERT_PRINT("\t" fmt "\n", ##__VA_ARGS__)
#endif /* CONFIG_ASSERT_NO_MSG_INFO */

#if !defined(CONFIG_ASSERT_NO_COND_INFO) && !defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                              \
	__ASSERT_PRINT("ASSERTION FAIL [%s] @ %s:%d\n", \
		Z_STRINGIFY(test),                      \
		__FILE__, __LINE__)
#endif

#if defined(CONFIG_ASSERT_NO_COND_INFO) && !defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                         \
	__ASSERT_PRINT("ASSERTION FAIL @ %s:%d\n", \
		__FILE__, __LINE__)
#endif

#if !defined(CONFIG_ASSERT_NO_COND_INFO) && defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                      \
	__ASSERT_PRINT("ASSERTION FAIL [%s]\n", \
		Z_STRINGIFY(test))
#endif

#if defined(CONFIG_ASSERT_NO_COND_INFO) && defined(CONFIG_ASSERT_NO_FILE_INFO)
#define __ASSERT_LOC(test)                 \
	__ASSERT_PRINT("ASSERTION FAIL\n")
#endif


#define __ASSERT_NO_MSG(test)                                             \
	do {                                                              \
		if (!(test)) {                                            \
			__ASSERT_LOC(test);                               \
			__ASSERT_POST_ACTION();                           \
		}                                                         \
	} while (false)

#define __ASSERT(test, fmt, ...)                                          \
	do {                                                              \
		if (!(test)) {                                            \
			__ASSERT_LOC(test);                               \
			__ASSERT_MSG_INFO(fmt, ##__VA_ARGS__);            \
			__ASSERT_POST_ACTION();                           \
		}                                                         \
	} while (false)

#define __ASSERT_EVAL(expr1, expr2, test, fmt, ...)                \
	do {                                                       \
		expr2;                                             \
		__ASSERT(test, fmt, ##__VA_ARGS__);                \
	} while (false)

#endif /* ZEPHYR_INCLUDE_SYS___ASSERT_H_ */
