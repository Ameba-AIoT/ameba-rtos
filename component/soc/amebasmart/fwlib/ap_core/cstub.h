/*
 * Copyright (c) 2020 Realtek Semiconductor Corp.	All rights reserved.
 *
 * Author: PSP Software Group
 */
#ifndef _CSTUB_H
#define _CSTUB_H

#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>

#define HEAP_START		__HeapBase
#define HEAP_END		__HeapLimit

#ifndef _ASMLANGUAGE

/* HEAP_START, HEAP_END are set in the linker command file */
extern caddr_t HEAP_START, HEAP_END;

#endif /* !_ASMLANGUAGE */

#endif
