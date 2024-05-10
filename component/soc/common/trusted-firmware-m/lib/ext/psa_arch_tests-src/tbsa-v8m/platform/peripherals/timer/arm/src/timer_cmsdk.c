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

#include "timer_cmsdk.h"

/**
    @brief    - Initializes an hardware timer
    @param    -  base_addr     : Base address of the timer module
                 time_us       : Time in micro seconds
                 timer_tick_us : Number of ticks per micro seconds
    @return   - SUCCESS/FAILURE
**/
int timer_cmsdk_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    /* Disable Timer */
    ((timer_t *)base_addr)->CTRL = 0x0;

    if (time_us == 0 ) {
        ((timer_t *)base_addr)->RELOAD = TIMER_MAX_VALUE;
    } else {
        ((timer_t *)base_addr)->RELOAD = time_us * timer_tick_us;
    }

    return 0;
}

/**
    @brief    - Enables a hardware timer
    @param    - base_addr: Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int timer_cmsdk_enable (addr_t base_addr)
{
    /* Enable Interrupt */
    ((timer_t *)base_addr)->CTRL = TIMER_CTRL_IRQEN_Msk;

    /* Enable Timer */
    ((timer_t *)base_addr)->CTRL |= TIMER_CTRL_EN_Msk;

    return 0;
}

/**
    @brief    - Disables a hardware timer
    @param    - base_addr: Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int timer_cmsdk_disable (addr_t base_addr)
{
    /* Disable Timer */
    ((timer_t *)base_addr)->CTRL     = 0x0;
    ((timer_t *)base_addr)->INTCLEAR = 0x1;
    return 0;
}

/**
    @brief    - Clears the interrupt status of timer
    @param    - base_addr: Base address of the timer module
    @return   - SUCCESS/FAILURE
**/
int timer_cmsdk_interrupt_clear (addr_t base_addr)
{
    ((timer_t *)base_addr)->INTCLEAR = 0x1;
    return 0;
}
