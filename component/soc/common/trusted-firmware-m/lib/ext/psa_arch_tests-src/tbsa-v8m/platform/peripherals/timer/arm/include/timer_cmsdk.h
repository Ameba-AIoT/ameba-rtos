/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
 * SPDX-License-Identifier : Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
**/

#ifndef _TIMER_CMSDK_H_
#define _TIMER_CMSDK_H_

#include "pal_common.h"

#define TIMER_CTRL_IRQEN_Pos          3                                        /* TIMER CTRL: IRQEN Position */
#define TIMER_CTRL_IRQEN_Msk          (0x01ul << TIMER_CTRL_IRQEN_Pos)         /* TIMER CTRL: IRQEN Mask */

#define TIMER_CTRL_SELEXTCLK_Pos      2                                        /* TIMER CTRL: SELEXTCLK Position */
#define TIMER_CTRL_SELEXTCLK_Msk      (0x01ul << TIMER_CTRL_SELEXTCLK_Pos)     /* TIMER CTRL: SELEXTCLK Mask */

#define TIMER_CTRL_SELEXTEN_Pos       1                                        /* TIMER CTRL: SELEXTEN Position */
#define TIMER_CTRL_SELEXTEN_Msk       (0x01ul << TIMER_CTRL_SELEXTEN_Pos)      /* TIMER CTRL: SELEXTEN Mask */

#define TIMER_CTRL_EN_Pos             0                                        /* TIMER CTRL: EN Position */
#define TIMER_CTRL_EN_Msk             (0x01ul << TIMER_CTRL_EN_Pos)            /* TIMER CTRL: EN Mask */

#define TIMER_VAL_CURRENT_Pos         0                                        /* TIMER VALUE: CURRENT Position */
#define TIMER_VAL_CURRENT_Msk         (0xFFFFFFFFul << TIMER_VAL_CURRENT_Pos)  /* TIMER VALUE: CURRENT Mask */

#define TIMER_RELOAD_VAL_Pos          0                                        /* TIMER RELOAD: RELOAD Position */
#define TIMER_RELOAD_VAL_Msk          (0xFFFFFFFFul << TIMER_RELOAD_VAL_Pos)   /* TIMER RELOAD: RELOAD Mask */

#define TIMER_INTSTATUS_Pos           0                                        /* TIMER INTSTATUS: INTSTATUSPosition */
#define TIMER_INTSTATUS_Msk           (0x01ul << TIMER_INTSTATUS_Pos)          /* TIMER INTSTATUS: INTSTATUSMask */

#define TIMER_INTCLEAR_Pos            0                                        /* TIMER INTCLEAR: INTCLEAR Position */
#define TIMER_INTCLEAR_Msk            (0x01ul << TIMER_INTCLEAR_Pos)           /* TIMER INTCLEAR: INTCLEAR Mask */


#define TIMER_MAX_VALUE               0xFFFFFFFF

typedef struct
{
    uint32_t  CTRL;            /* Offset: 0x000 (R/W) Control Register */
    uint32_t  VALUE;           /* Offset: 0x004 (R/W) Current Value Register */
    uint32_t  RELOAD;          /* Offset: 0x008 (R/W) Reload Value Register */
    union {
        uint32_t  INTSTATUS;   /* Offset: 0x00C (R/ ) Interrupt Status Register */
        uint32_t  INTCLEAR;    /* Offset: 0x00C ( /W) Interrupt Clear Register */
    };
} timer_t;

int timer_cmsdk_init    (addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int timer_cmsdk_enable  (addr_t base_addr);
int timer_cmsdk_disable (addr_t base_addr);
int timer_cmsdk_interrupt_clear (addr_t base_addr);

#endif /* _TIMER_CMSDK_H_ */
