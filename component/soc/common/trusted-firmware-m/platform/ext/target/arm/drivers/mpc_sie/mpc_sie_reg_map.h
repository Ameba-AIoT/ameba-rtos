/*
 * Copyright (c) 2021 ARM Limited
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * \file mpc_sie_reg_map.h
 * \brief Register map for ARM SIE Memory Protection
 *        Controllers (MPC).
 */

#ifndef __MPC_SIE_REG_MAP_H__
#define __MPC_SIE_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ARM MPC memory mapped register access structure */
struct mpc_sie_reg_map_t {
    volatile uint32_t ctrl;       /* (R/W) MPC Control */
    volatile uint32_t reserved[3];/* Reserved */
    volatile uint32_t blk_max;    /* (R/ ) Maximum value of block based index */
    volatile uint32_t blk_cfg;    /* (R/ ) Block configuration */
    volatile uint32_t blk_idx;    /* (R/W) Index value for accessing block
                                   *       based look up table
                                   */
    volatile uint32_t blk_lutn;   /* (R/W) Block based gating
                                   *       Look Up Table (LUT)
                                   */
    volatile uint32_t int_stat;   /* (R/ ) Interrupt state */
    volatile uint32_t int_clear;  /* ( /W) Interrupt clear */
    volatile uint32_t int_en;     /* (R/W) Interrupt enable */
    volatile uint32_t int_info1;  /* (R/ ) Interrupt information 1 */
    volatile uint32_t int_info2;  /* (R/ ) Interrupt information 2 */
    volatile uint32_t int_set;    /* ( /W) Interrupt set. Debug purpose only */
    volatile uint32_t reserved2[998]; /* Reserved */
    volatile uint32_t pidr4;      /* (R/ ) Peripheral ID 4 */
    volatile uint32_t pidr5;      /* (R/ ) Peripheral ID 5 */
    volatile uint32_t pidr6;      /* (R/ ) Peripheral ID 6 */
    volatile uint32_t pidr7;      /* (R/ ) Peripheral ID 7 */
    volatile uint32_t pidr0;      /* (R/ ) Peripheral ID 0 */
    volatile uint32_t pidr1;      /* (R/ ) Peripheral ID 1 */
    volatile uint32_t pidr2;      /* (R/ ) Peripheral ID 2 */
    volatile uint32_t pidr3;      /* (R/ ) Peripheral ID 3 */
    volatile uint32_t cidr0;      /* (R/ ) Component ID 0 */
    volatile uint32_t cidr1;      /* (R/ ) Component ID 1 */
    volatile uint32_t cidr2;      /* (R/ ) Component ID 2 */
    volatile uint32_t cidr3;      /* (R/ ) Component ID 3 */
};

#ifdef __cplusplus
}
#endif

#endif /* __MPC_SIE_REG_MAP_H__ */
