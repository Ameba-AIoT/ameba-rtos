/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _DEBUG_H
#define _DEBUG_h

#ifndef _ASMLANGUAGE

#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#define debug_printf(fmt, ...)	printf(fmt, ##__VA_ARGS__)
#else
#define debug_printf(fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* !_ASMLANGUAGE */

#endif
