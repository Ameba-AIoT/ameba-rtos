/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SW_UTIL_H_
#define SW_UTIL_H_

#include <stdlib.h>
#include "basetype.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef STACK_STAT
void StackStatInit();
void StackConsumption(const char *);
#else
#define StackStatInit()
#define StackConsumption(str)
#endif

#ifdef __cplusplus
}
#endif

#endif /* #ifdef SW_UTIL_H_ */
