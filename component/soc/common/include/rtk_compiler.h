/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RTK_COMPILER_H__
#define __RTK_COMPILER_H__

#ifdef __ZEPHYR__
#include <zephyr/toolchain.h>
#endif

#define Compile_Assert(exp, str) extern char __ct_[(exp) ? 1 : -1]

#define ALIGNMTO(_bound)    __attribute__ ((aligned (_bound)))
#define SECTION(_name)      __attribute__ ((__section__(_name)))
#define _WEAK               __attribute__ ((weak))
#define _PACKED_            __attribute__ ((packed))
#define __NO_INLINE         __attribute__ ((noinline))

#if defined (__GNUC__) && !defined (__clang__) && defined (__arm__) && !defined (CONFIG_BUILD_ROM)
#define _LONG_CALL_     __attribute__ ((long_call))
#else
#define _LONG_CALL_
#endif
#if defined (__clang__)
/* clang has no per function optimize attribute */
#define _OPTIMIZE_NONE_           __attribute__ ((optnone))
#define _OPTIMIZE_O3_
#else
#define _OPTIMIZE_NONE_           __attribute__ ((optimize("O0")))
#define _OPTIMIZE_O3_           __attribute__ ((optimize("O3")))
#endif
#define UNUSED_WARN_DIS	__attribute__((unused))

/* GNUC */
#ifdef __GNUC__

#ifndef __weak
#define __weak __attribute__((weak))
#endif

#ifndef likely
#define likely(x) __builtin_expect((x), 1)
#endif

#ifndef unlikely
#define unlikely(x) __builtin_expect((x), 0)
#endif

/* inline function */
#define __inline__                      inline
#define __inline                        inline
#define __inline_definition		inline

#ifndef   __STATIC_INLINE
#define __STATIC_INLINE           static inline   __attribute__((always_inline))
#endif

/* pack */
#define RTW_PACK_STRUCT_BEGIN
#define RTW_PACK_STRUCT_STRUCT __attribute__ ((__packed__))
#define RTW_PACK_STRUCT_END

#endif

#endif
