/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AONTIMER_H_
#define _AMEBA_AONTIMER_H_

extern void AONTimer_Setting(u32 SDuration);
extern void AONTimer_INT(u32 Status);
extern u32 AONTimer_Get(void);
extern void AONTimer_ClearINT(void);
extern void AONTimer_INT_WAKEUP(void);
#endif  //_AMEBA_AONTIMER_H_
