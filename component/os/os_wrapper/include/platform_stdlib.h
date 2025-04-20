/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PLATFORM_STDLIB_H__
#define __PLATFORM_STDLIB_H__

#include <stdio.h>

/*NOTE: types.h is include by stdio.h in toolchain 10.3.1, but not in 12.3.1. So include it here*/
#include <sys/types.h> /* For using BYTE_ORDER, LITTLE_ENDIAN in endian.h */
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdarg.h> /* va_list */

#endif //__PLATFORM_STDLIB_H__

