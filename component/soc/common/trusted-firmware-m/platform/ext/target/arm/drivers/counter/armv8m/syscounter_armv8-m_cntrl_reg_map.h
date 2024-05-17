
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
 * \file syscounter_armv8-m_cntrl_reg_map.h
 * \brief Armv8-M System Counter Control, covering CNTControlBase Frame
 */

#ifndef __SYSCOUNTER_ARMV8_M_CNTRL_REG_MAP_H__
#define __SYSCOUNTER_ARMV8_M_CNTRL_REG_MAP_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief CNTControlBase Register map structure
 */
struct cnt_control_base_reg_map_t {
    volatile uint32_t cntcr;
        /*!< Offset: 0x000 (R/W) Counter Control Register */
    volatile const uint32_t cntsr;
        /*!< Offset: 0x004 (RO) Counter Status Register */
    volatile uint32_t cntcv_low;
        /*!< Offset: 0x008 (R/W) Counter Count Value [31:0] Register */
    volatile uint32_t cntcv_high;
        /*!< Offset: 0x00C (R/W) Counter Count Value [63:32] Register */
    volatile uint32_t cntscr;
        /*!< Offset: 0x010 (R/W) Counter Scale Register
         *   Aliased with CNTSCR0, meaning that either addresses of CNTSCR and
         *   CNTSCR0 will physically access a single register
         */
    volatile const uint32_t reserved0[2];
        /*!< Offset: 0x014-0x018 Reserved (RAZWI) */
    volatile const uint32_t cntid;
        /*!< Offset: 0x01C (RO) Counter ID Register */
    volatile const uint32_t reserved1[40];
        /*!< Offset: 0x020-0x0BC Reserved (RAZWI) */
    volatile const uint32_t reserved2[4];
        /*!< Offset: 0x0C0-0x0CC Reserved (RAZWI) */
    volatile uint32_t cntscr0;
        /*!< Offset: 0x0D0 (R/W) Counter Scale Register 0 */
    volatile uint32_t cntscr1;
        /*!< Offset: 0x0D4 (R/W) Counter Scale Register 1 */
    volatile const uint32_t reserved3[958];
        /*!< Offset: 0x0D8-0xFCC Reserved (RAZWI) */
    volatile const uint32_t cntpidr4;
        /*!< Offset: 0xFD0 (RO) Peripheral ID Register */
    volatile const uint32_t reserved4[3];
        /*!< Offset: 0xFD4-0xFDC Reserved (RAZWI) */
    volatile const uint32_t cntpidr0;
        /*!< Offset: 0xFE0 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr1;
        /*!< Offset: 0xFE4 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr2;
        /*!< Offset: 0xFE8 (RO) Peripheral ID Register */
    volatile const uint32_t cntpidr3;
        /*!< Offset: 0xFEC (RO) Peripheral ID Register */
    volatile const uint32_t cntcidr0;
        /*!< Offset: 0xFF0 (RO) Component ID Register */
    volatile const uint32_t cntcidr1;
        /*!< Offset: 0xFF4 (RO) Component ID Register */
    volatile const uint32_t cntcidr2;
        /*!< Offset: 0xFF8 (RO) Component ID Register */
    volatile const uint32_t cntcidr3;
        /*!< Offset: 0xFFC (RO) Component ID Register */
};

#ifdef __cplusplus
}
#endif

#endif /* __SYSCOUNTER_ARMV8_M_CNTRL_REG_MAP_H__ */
