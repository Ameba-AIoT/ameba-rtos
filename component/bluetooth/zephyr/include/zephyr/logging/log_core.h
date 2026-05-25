/*
 * Copyright (c) 2018 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: Apache-2.0
 */
#ifndef ZEPHYR_INCLUDE_LOGGING_LOG_CORE_H_
#define ZEPHYR_INCLUDE_LOGGING_LOG_CORE_H_

#include <zephyr/logging/log_msg.h>
#include <zephyr/logging/log_instance.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdarg.h>
#include <zephyr/sys/util.h>
#include <zephyr_log.h>

/* This header file keeps all macros and functions needed for creating logging
 * messages (macros like @ref LOG_ERR).
 */

#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
/****************** Definitions used by minimal logging *********************/
/*****************************************************************************/
static inline char z_log_minimal_level_to_char(int level)
{
	switch (level) {
	case LOG_LEVEL_ERR:
		return 'E';
	case LOG_LEVEL_WRN:
		return 'W';
	case LOG_LEVEL_INF:
		return 'I';
	case LOG_LEVEL_DBG:
		return 'D';
	default:
		return '?';
	}
}

#if defined(CONFIG_LOG) && CONFIG_LOG

#if (ZLOG_STACK_MODE == ZLOG_STACK_MODE_PRINTF)
#define Z_LOG_PRINT_DETAIL(_level, ...)	\
	do {	\
		z_printf("[%c:%s] ", z_log_minimal_level_to_char(_level), __func__);	\
		z_printf(__VA_ARGS__);	\
		z_printf("\r\n");		\
	} while(0)

#define Z_LOG_PRINT_HEXDUMP(_data, _length)	\
	do {	\
		int __dump_idx;	\
		for (__dump_idx = 0; __dump_idx < _length; __dump_idx++) {	\
			z_printf("%02X ", *((char *)_data + __dump_idx));	\
			if ((__dump_idx + 1) % 16 == 0) {	\
				z_printf("\r\n");	\
			}	\
		}	\
		if (_length % 16 != 0) {	\
			z_printf("\r\n");	\
		}	\
	} while(0)

#else /* ZLOG_STACK_MODE == ZLOG_STACK_MODE_PRINTF */

#define TRACE_STRING_DATA __attribute__((section(".BTTRACE"))) __attribute__((aligned(4))) __attribute__((used))
#define __count_args(_0,  _1,  _2,  _3,  _4,  _5,  _6,  _7,  _8,  _9,  _10, _11, _12, _13, _14, _15,\
					 _16, _17, _18, _19, _20, _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, _31,\
					 _32, _33, _34, _35, _36, _37, _38, _39, _40, _41, _42, _43, _44, _45, _46, _47,\
					 _48, _49, _50, _51, _52, _53, _54, _55, _56, _57, _58, _59, _60, _61, _62, _63, N, ...) N
#define Z_count_args(...) \
	__count_args(__VA_ARGS__, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, \
							  47, 46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, \
							  31, 30, 29, 28, 27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, \
							  15, 14, 13, 12, 11, 10, 9,  8,  7,  6,  5,  4,  3,  2,  1,  0)
#define _line_to_str(A) #A
#define z_line_to_str(A) _line_to_str(A)
#define Z_LOG_PRINT_DETAIL(_level, fmt, ...)	\
	do {static const char __log_string[] TRACE_STRING_DATA = "["__FILE_Z_STR__":"z_line_to_str(__LINE__)"]"fmt"\r\n";	\
		zephyr_stacklog_args_send(_level, (int)__log_string, Z_count_args(fmt, ##__VA_ARGS__), ##__VA_ARGS__); \
	} while(0)

#define Z_LOG_PRINT_HEXDUMP(_data, _length)	\
	do {	\
		int __dump_idx;	\
		for (__dump_idx = 0; __dump_idx < _length; __dump_idx +=16) {	\
			zephyr_stacklog_args_send(0, 0, (_length - __dump_idx < 16) ? (_length - __dump_idx) : 16, (char *)_data + __dump_idx);	\
		}	\
	} while(0)

#endif /* ZLOG_STACK_MODE == ZLOG_STACK_MODE_PRINTF */

#define Z_LOG(_level, ...) \
	do {					\
		if (!(_level > zlog_level_get())) {				\
			Z_LOG_PRINT_DETAIL(_level, __VA_ARGS__);	\
		}	\
	} while(0)


#define Z_LOG_HEXDUMP(_level, _data, _length, ...) \
	do {					\
		if (!(_level > zlog_level_get())) {				\
			Z_LOG_PRINT_DETAIL(_level, __VA_ARGS__);	\
			Z_LOG_PRINT_HEXDUMP(_data, _length);	\
		}	\
	} while(0)

#else /* defined(CONFIG_LOG) && CONFIG_LOG */
#define Z_LOG_PRINT_DETAIL(_level, fmt, ...) { }
#define Z_LOG(_level, ...) { }
#define Z_LOG_HEXDUMP(_level, _data, _length, ...) { }
#endif /* defined(CONFIG_LOG) && CONFIG_LOG */

#ifdef __cplusplus
}
#endif

#endif /* ZEPHYR_INCLUDE_LOGGING_LOG_CORE_H_ */
