/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __PPDEBUG_H__
#define __PPDEBUG_H__

/* macro for assertion, used only when _ASSERT_USED is defined */
#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

/* macro for debug printing, used only when _DEBUG_PRINT is defined */
#ifdef _PPDEBUG_PRINT
#include <stdio.h>
#define PPDEBUG_PRINT(args) printf args
#else
#define PPDEBUG_PRINT(args)
#endif


#endif /* __PPDEBUG_H__ */
