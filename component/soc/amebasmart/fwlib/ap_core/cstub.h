/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _CSTUB_H
#define _CSTUB_H

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define HEAP_START		__HeapBase
#define HEAP_END		__HeapLimit

#ifndef _ASMLANGUAGE

#ifdef __cplusplus
extern "C" {
#endif

/* HEAP_START, HEAP_END are set in the linker command file */
extern caddr_t HEAP_START, HEAP_END;

#ifdef __cplusplus
}
#endif

#endif /* !_ASMLANGUAGE */

#endif
