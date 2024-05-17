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
 * \file systimer_armv8-m_reg_map.h
 *
 * \brief Register map for Armv8-M System Timer
 */

#ifndef __SYSTIMER_ARMV8_M_REG_MAP_H__
#define __SYSTIMER_ARMV8_M_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief CNTBase Register map structure
 */
struct cnt_base_reg_map_t {
    volatile const uint32_t cntpct_low;
        /*!< Offset: 0x000 (RO) Current Physical Counter Value [31:0] */
    volatile const uint32_t cntpct_high;
        /*!< Offset: 0x004 (RO) Current Physical Counter Value [63:32] */
    volatile const uint32_t reserved0[2];
        /*!< Offset: 0x008-0x0C Reserved  */
    volatile uint32_t cntfrq;
        /*!< Offset: 0x010 (R/W) Counter Frequency register in Hz */
    volatile const uint32_t reserved1[3];
        /*!< Offset: 0x014-0x01C Reserved  */
    volatile uint32_t cntp_cval_low;
        /*!< Offset: 0x020 (R/W) Timer Compare Value register [31:0] */
    volatile uint32_t cntp_cval_high;
        /*!< Offset: 0x024 (R/W) Timer Compare Value register [63:32] */
    volatile uint32_t cntp_tval;
        /*!< Offset: 0x028 (R/W) Timer Value register */
    volatile uint32_t cntp_ctl;
        /*!< Offset: 0x02C (R/W) Timer Control register */
    volatile const uint32_t reserved2[4];
        /*!< Offset: 0x030-0x03C Reserved  */
    volatile const uint32_t cntp_aival_low;
        /*!< Offset: 0x040 (RO) Auto Increment Value register [31:0]*/
    volatile const  uint32_t cntp_aival_high;
        /*!< Offset: 0x044 (RO) Auto Increment Value register [63:32]*/
    volatile uint32_t cntp_aival_reload;
        /*!< Offset: 0x048 (R/W) Auto Increment Value Reload register [63:32]*/
    volatile uint32_t cntp_aival_ctl;
        /*!< Offset: 0x04C (R/W) Auto Increment Control register */
    volatile const  uint32_t cntp_cfg;
        /*!< Offset: 0x050 (RO) Timer Configuration register */
    volatile const uint32_t reserved3[991];
        /*!< Offset: 0x054-0xFCC Reserved  */
    volatile const uint32_t cntp_pid4;
        /*!< Offset: 0xFD0 (RO) Peripheral ID Register */
    volatile const uint32_t reserved4[3];
        /*!< Offset: 0xFD4-0xFDC Reserved (RAZWI) */
    volatile const uint32_t cntp_pid0;
        /*!< Offset: 0xFE0 (RO) Peripheral ID Register */
    volatile const uint32_t cntp_pid1;
        /*!< Offset: 0xFE4 (RO) Peripheral ID Register */
    volatile const uint32_t cntp_pid2;
        /*!< Offset: 0xFE8 (RO) Peripheral ID Register */
    volatile const uint32_t cntp_pid3;
        /*!< Offset: 0xFEC (RO) Peripheral ID Register */
    volatile const uint32_t cntp_cid0;
        /*!< Offset: 0xFF0 (RO) Component ID Register */
    volatile const uint32_t cntp_cid1;
        /*!< Offset: 0xFF4 (RO) Component ID Register */
    volatile const uint32_t cntp_cid2;
        /*!< Offset: 0xFF8 (RO) Component ID Register */
    volatile const uint32_t cntp_cid3;
        /*!< Offset: 0xFFC (RO) Component ID Register */
};

#ifdef __cplusplus
}
#endif

#endif /* __SYSTIMER_ARMV8_M_REG_MAP_H__ */
