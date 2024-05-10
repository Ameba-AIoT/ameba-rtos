/*
 * Copyright (c) 2019-2023, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "stdarg.h"
#include "string.h"
#include "stdint.h"
#include "cc_pal_log.h"

/* Log level = debug */
int CC_PAL_logLevel = 3;
/* Don't filter on components */
uint32_t CC_PAL_logMask = 0xFFFFFFFF;

void CC_PalLogInit(void)
{
    return;
}

void CC_PalLog(int level, const char* format, ...)
{
    (void) level;

    va_list args;

    va_start(args, format);
    vprintf(format, args);
    va_end(args);
}
