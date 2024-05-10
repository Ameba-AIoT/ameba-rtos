/** @file
 * Copyright (c) 2020, Arm Limited or its affiliates. All rights reserved.
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

#include "pal_wd_syswdog.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFF

struct wdog_frame_reg_map_t {
/* Control Frame registers */
    volatile uint32_t wcs;
        /*!< Offset: 0x0000 (RW) Watchdog Control and Status register */
    volatile uint32_t reserved0;
        /*!< Offset: 0x0004 (RES0) Reserved */
    volatile uint32_t wor;
        /*!< Offset: 0x0008 (RW) Watchdog Offset register */
    volatile uint32_t reserved1;
        /*!< Offset: 0x000C (RES0) Reserved */
    volatile uint32_t wcv_low;
        /*!< Offset: 0x0010 (RW) Watchdog Compare Value low register [31:0] */
    volatile uint32_t wcv_high;
        /*!< Offset: 0x0014 (RW) Watchdog Compare Value high register [63:32] */
    volatile uint32_t reserved2[1005];
        /*!< Offset: 0x0018 (RES0) Reserved */
    volatile uint32_t cnt_w_iidr;
        /*!< Offset: 0xFCC (RO) Watchdog Interface Identification register */
    volatile uint32_t reserved3[12];
        /*!< Offset: 0x0FD0 (RES0) Reserved */
/* Refresh Frame registers */
    volatile uint32_t wrr;
        /*!< Offset: 0x1000 (RW) Watchdog Refresh register */
    volatile uint32_t reserved4[1010];
        /*!< Offset: 0x1004 (RES0) Reserved */
    volatile uint32_t ref_w_iidr;
        /*!< Offset: 0x1FCC (RW) Watchdog Interface Identification register */
    volatile uint32_t reserved5[12];
        /*!< Offset: 0x1FD0 (RES0) Reserved */
};

/*!< System Watchdog registers bit width */
#define SYSWDOG_REGISTER_BIT_WIDTH          32u

/* \brief Watchdog Control and Status register bit fields */
#define SYSWDOG_CNTR_WCS_EN_OFF    0u    /*!< Control and Status register Watchdog Enable bit field offset */
#define SYSWDOG_CNTR_WCS_WS0_OFF   1u    /*!< Control and Status register Watchdog Signal 0 bit field offset */
#define SYSWDOG_CNTR_WCS_WS1_OFF   2u    /*!< Control and Status register Watchdog Signal 1 bit field offset */

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_syswdog_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    struct wdog_frame_reg_map_t* p_cntr = (struct wdog_frame_reg_map_t*)base_addr;

    /* Set offset register */
    p_cntr->wor = time_us * timer_tick_us;

    /* Enable watchdog */
    p_cntr->wcs |= SYSWDOG_CNTR_WCS_EN_OFF;

    return 0;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_syswdog_enable(addr_t base_addr)
{
    struct wdog_frame_reg_map_t* p_cntr = (struct wdog_frame_reg_map_t*)base_addr;

    /* Enable watchdog */
    p_cntr->wcs |= SYSWDOG_CNTR_WCS_EN_OFF;

    return 0;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_syswdog_disable(addr_t base_addr)
{
    struct wdog_frame_reg_map_t* p_cntr = (struct wdog_frame_reg_map_t*)base_addr;

    /* Disable Watchdog */
    p_cntr->wcs &= ~(SYSWDOG_CNTR_WCS_EN_OFF);

    /* Clear interrupts */
    p_cntr->wcs &= ~(SYSWDOG_CNTR_WCS_WS0_OFF);
    p_cntr->wcs &= ~(SYSWDOG_CNTR_WCS_WS1_OFF);

    return 0;
}

/**
    @brief           - Checks whether hardware watchdog timer is enabled
    @param           - base_addr       : Base address of the watchdog module
    @return          - Enabled : 1, Disabled : 0
**/
int pal_wd_syswdog_is_enabled(addr_t base_addr)
{
    struct wdog_frame_reg_map_t* p_cntr = (struct wdog_frame_reg_map_t*)base_addr;

    return (((p_cntr->wcs) & SYSWDOG_CNTR_WCS_EN_OFF) ? 1 : 0);
}
