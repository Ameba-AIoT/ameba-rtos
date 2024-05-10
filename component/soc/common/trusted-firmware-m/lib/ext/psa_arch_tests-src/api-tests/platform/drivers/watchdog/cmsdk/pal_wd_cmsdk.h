/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _PAL_WD_CMSDK_H_
#define _PAL_WD_CMSDK_H_

#include "pal_common.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFF

typedef struct {
    uint32_t  LOAD;          /* Offset: 0x000 (R/W) Watchdog Load Register */
    uint32_t  VALUE;         /* Offset: 0x004 (R/ ) Watchdog Value Register */
    uint32_t  CTRL;          /* Offset: 0x008 (R/W) Watchdog Control Register */
    uint32_t  INTCLR;        /* Offset: 0x00C ( /W) Watchdog Clear Interrupt Register */
    uint32_t  RAWINTSTAT;    /* Offset: 0x010 (R/ ) Watchdog Raw Interrupt Status Register */
    uint32_t  MASKINTSTAT;   /* Offset: 0x014 (R/ ) Watchdog Interrupt Status Register */
    uint32_t  RESERVED0[762];
    uint32_t  LOCK;          /* Offset: 0xC00 (R/W) Watchdog Lock Register */
    uint32_t  RESERVED1[191];
    uint32_t  ITCR;          /* Offset: 0xF00 (R/W) Watchdog Integration Test Control Register */
    uint32_t  ITOP;          /* Offset: 0xF04 ( /W) Watchdog Integration Test Output Set Register */
} wd_timer_t;

/* WATCHDOG LOAD Register Definitions */
#define Watchdog_LOAD_Pos               0               /* Watchdog LOAD: LOAD Position */
#define Watchdog_LOAD_Msk              (0xFFFFFFFFUL)   /* Watchdog LOAD: LOAD Mask */

/* WATCHDOG VALUE Register Definitions */
#define Watchdog_VALUE_Pos              0              /* Watchdog VALUE: VALUE Position */
#define Watchdog_VALUE_Msk             (0xFFFFFFFFUL)  /* Watchdog VALUE: VALUE Mask */

/* WATCHDOG CTRL Register Definitions */
/* Watchdog CTRL_RESEN: Enable Reset Output Position */
#define Watchdog_CTRL_RESEN_Pos         1
 /* Watchdog CTRL_RESEN: Enable Reset Output Mask */
#define Watchdog_CTRL_RESEN_Msk        (0x1UL << Watchdog_CTRL_RESEN_Pos)

#define Watchdog_CTRL_INTEN_Pos         0          /* Watchdog CTRL_INTEN: Int Enable Position */
#define Watchdog_CTRL_INTEN_Msk        (0x1UL)    /* Watchdog CTRL_INTEN: Int Enable Mask */

/* WATCHDOG INTCLR Register Definitions */
#define Watchdog_INTCLR_Pos             0          /* Watchdog INTCLR: Int Clear Position */
#define Watchdog_INTCLR_Msk            (0x1UL)     /* Watchdog INTCLR: Int Clear Mask */

/* WATCHDOG RAWINTSTAT Register Definitions */
#define Watchdog_RAWINTSTAT_Pos         0         /* Watchdog RAWINTSTAT: Raw Int Status Position */
#define Watchdog_RAWINTSTAT_Msk        (0x1UL)    /* Watchdog RAWINTSTAT: Raw Int Status Mask */

/* WATCHDOG MASKINTSTAT Register Definitions */
#define Watchdog_MASKINTSTAT_Pos        0       /* Watchdog MASKINTSTAT: Mask Int Status Position */
#define Watchdog_MASKINTSTAT_Msk       (0x1UL)  /* Watchdog MASKINTSTAT: Mask Int Status Mask */

/* WATCHDOG LOCK Register Definitions */
#define Watchdog_LOCK_Pos               0       /* Watchdog LOCK: LOCK Position */
#define Watchdog_LOCK_Msk              (0x1UL)  /* Watchdog LOCK: LOCK Mask */

/* WATCHDOG INTEGTESTEN Register Definitions */
#define Watchdog_INTEGTESTEN_Pos        0     /* Watchdog INTEGTESTEN: Integration Test Enable Pos*/
#define Watchdog_INTEGTESTEN_Msk       (0x1UL)/*Watchdog INTEGTESTEN: Integration Test Enable Mask*/

/* WATCHDOG INTEGTESTOUTSET Register Definitions */
/* Watchdog INTEGTESTOUTSET: Integration Test Output Set Position */
#define Watchdog_INTEGTESTOUTSET_Pos    1
/* Watchdog INTEGTESTOUTSET: Integration Test Output Set Mask */
#define Watchdog_INTEGTESTOUTSET_Msk   (0x1UL)

int pal_wd_cmsdk_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int pal_wd_cmsdk_enable(addr_t base_addr);
int pal_wd_cmsdk_disable(addr_t base_addr);
int pal_wd_cmsdk_is_enabled(addr_t base_addr);

#endif /* PAL_WD_CMSDK_H */
