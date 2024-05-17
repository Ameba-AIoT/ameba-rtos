/*
 * Copyright (c) 2024 Realtek Semiconductor Corp.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef _AMEBA_AONTIMER_H_
#define _AMEBA_AONTIMER_H_

extern void SOCPS_AONTimer(u32 SDuration);
extern void SOCPS_AONTimerINT_EN(u32 Status);
extern u32 SOCPS_AONTimerGet(void);
extern void SOCPS_AONTimerClearINT(void);

#endif  //_AMEBA_AONTIMER_H_