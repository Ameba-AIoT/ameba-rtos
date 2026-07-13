/*
 * FreeRTOS Kernel V11.3.0
 * Copyright (C) 2021 Amazon.com, Inc. or its affiliates. All Rights Reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */

#ifndef INC_NEWLIB_FREERTOS_H
#define INC_NEWLIB_FREERTOS_H

/* Note Newlib support has been included by popular demand, but is not
 * used by the FreeRTOS maintainers themselves.  FreeRTOS is not
 * responsible for resulting newlib operation.  User must be familiar with
 * newlib and must provide system-wide implementations of the necessary
 * stubs. Be warned that (at the time of writing) the current newlib design
 * implements a system-wide malloc() that must be provided with locks.
 *
 * See the third party link http://www.nadler.com/embedded/newlibAndFreeRTOS.html
 * for additional information. */

#include <reent.h>

#define configUSE_C_RUNTIME_TLS_SUPPORT    1

#if (configNUMBER_OF_CORES > 1) && !defined(__DYNAMIC_REENT__)
    #error Missing definition: __DYNAMIC_REENT__ must be enabled in the toolchain on SMP system.
#endif

#ifndef configTLS_BLOCK_TYPE
    #define configTLS_BLOCK_TYPE           struct _reent
#endif

#ifndef configINIT_TLS_BLOCK
    #define configINIT_TLS_BLOCK( xTLSBlock, pxTopOfStack )    _REENT_INIT_PTR( &( xTLSBlock ) )
#endif

/* When using __DYNAMIC_REENT__, newlib obtains the reent structure by calling
 * __getreent() (provided by locks.c) on every C-library entry point.
 * configDEINIT_TLS_BLOCK is set to no-op because _reclaim_reent must not be
 * called when newlib manages the reent lifetime through __getreent().
 * configSET_TLS_BLOCK still writes _impure_ptr (harmless but consistent with
 * v11.1.0 behaviour — newlib ignores it when __DYNAMIC_REENT__ is active). */
#ifdef __DYNAMIC_REENT__
    #define configDEINIT_TLS_BLOCK( xTLSBlock )
#endif

#ifndef configSET_TLS_BLOCK
    #define configSET_TLS_BLOCK( xTLSBlock )    ( _impure_ptr = &( xTLSBlock ) )
#endif

#ifndef configDEINIT_TLS_BLOCK
    #define configDEINIT_TLS_BLOCK( xTLSBlock )    _reclaim_reent( &( xTLSBlock ) )
#endif

/*
 * This function is used by the Newlib C library port layer.
 * Get the reentrant structure for the running task.
 * Declared with C linkage so that it matches the declaration in task.h
 * (which is inside an extern "C" block) and avoids linkage conflicts in C++. */
#ifdef __cplusplus
extern "C" {
#endif
struct _reent * pxTaskGetReent( void );
#ifdef __cplusplus
}
#endif

#endif /* INC_NEWLIB_FREERTOS_H */
