/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef __RTK_COMPILER_H__
#define __RTK_COMPILER_H__

#define Compile_Assert(exp, str) extern char __ct_[(exp) ? 1 : -1]

#define ALIGNMTO(_bound)    __attribute__ ((aligned (_bound)))
#define SECTION(_name)      __attribute__ ((__section__(_name)))
#define _WEAK               __attribute__ ((weak))
#define _PACKED_            __attribute__ ((packed))
#define __NO_INLINE         __attribute__ ((noinline))

/* ARM Compiler 6 */
#if defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050)
#define _LONG_CALL_
#else
#if defined (RISC_V)
#define _LONG_CALL_
#else
#define _LONG_CALL_     __attribute__ ((long_call))
#endif
#define _OPTIMIZE_NONE_           __attribute__ ((optimize("O0")))
#define _OPTIMIZE_O3_           __attribute__ ((optimize("O3")))
#define UNUSED_WARN_DIS	__attribute__((unused))
#endif

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
