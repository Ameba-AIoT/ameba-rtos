/*
 * Copyright (c) 2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#ifndef __PLATFORM_LOG_H__
#define __PLATFORM_LOG_H__

#define PLAT_LOG_LEVEL_OFF       (0)
#define PLAT_LOG_LEVEL_ERROR     (1)
#define PLAT_LOG_LEVEL_WARN      (2)
#define PLAT_LOG_LEVEL_INFO      (3)
#define PLAT_LOG_LEVEL_DEBUG     (4)

#ifndef PLAT_LOG_MODULE_NAME
#define MODULE_NAME_STR " "
#else
#define MODULE_NAME_STR "["PLAT_LOG_MODULE_NAME"]: "
#endif

#ifndef PLAT_LOG_LEVEL
#warning "Logging level is not defined, default is PLAT_LOG_LEVEL_ERROR."
#define PLAT_LOG_LEVEL   PLAT_LOG_LEVEL_ERROR
#endif


/* System can override PRINTF with other rich format function*/
#ifndef PRINTF
#if PLAT_LOG_LEVEL > PLAT_LOG_LEVEL_OFF
#include <stdio.h>
#define PRINTF printf
#endif
#endif

#if PLAT_LOG_LEVEL >= PLAT_LOG_LEVEL_ERROR
    #define ERROR(f_, ...) do { PRINTF("\033[31;4m[ERR]:\033[m%s"f_"\r\n", MODULE_NAME_STR, ##__VA_ARGS__);  } while (0)
#else
    #define ERROR(f_, ...) do {  } while(0)
#endif
#if PLAT_LOG_LEVEL >= PLAT_LOG_LEVEL_WARN
    #define WARN(f_, ...) do { PRINTF("\033[33;4m[WRN]:\033[m%s"f_"\r\n", MODULE_NAME_STR, ##__VA_ARGS__);  } while (0)
#else
    #define WARN(f_, ...) do {  } while(0)
#endif
#if PLAT_LOG_LEVEL >= PLAT_LOG_LEVEL_INFO
    #define INFO(f_, ...) do { PRINTF("[INF]:%s"f_"\r\n", MODULE_NAME_STR, ##__VA_ARGS__);  } while (0)
#else
    #define INFO(f_, ...) do {  } while(0)
#endif
#if PLAT_LOG_LEVEL >= PLAT_LOG_LEVEL_DEBUG
    #define VERBOSE(f_, ...) do { PRINTF("[DBG]:%s" f_"\r\n",MODULE_NAME_STR, ##__VA_ARGS__);  } while (0)
    #define DEBUG(f_, ...) do { PRINTF("[DBG]:%s" f_"\r\n",MODULE_NAME_STR, ##__VA_ARGS__);  } while (0)
#else
    #define VERBOSE(f_, ...) do {  } while(0)
    #define DEBUG(f_, ...) do {  } while(0)
#endif

#endif /* __PLATFORM_LOG_H__ */
