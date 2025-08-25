/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __JPEGDEBUG_H__
#define __JPEGDEBUG_H__

#ifdef _ASSERT_USED
#ifndef ASSERT
#include <assert.h>
#define ASSERT(expr) assert(expr)
#endif
#else
#define ASSERT(expr)
#endif

#ifdef _JPEGDECAPITRACE
#include <stdio.h>
#elif _JPEGDEC_TRACE
#include <stdio.h>
#elif _JPEGDEC_API_TRC
#include <stdio.h>
#elif _JPEGDEC_PP_TRACE
#include <stdio.h>
#elif _DEBUG
#include <stdio.h>
#elif _DEBUG
#include <stdio.h>
#endif

/* macro for debug printing. Note that double parenthesis has to be used, i.e.
 * DEBUG(("Debug printing %d\n",%d)) */
#ifdef _JPEGAPITRACE
#define JPEG_API_TRC(args) printf args
#else
#define JPEG_API_TRC(args)
#endif

#ifdef _DEBUG
#define DEBUG(args) printf args
#else
#define DEBUG(args)
#endif


#endif
