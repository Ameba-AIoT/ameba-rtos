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

#include "pal_wd_rtk.h"

/**
    @brief           - Initializes an hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
                     - time_us         : Time in micro seconds
                     - timer_tick_us   : Number of ticks per micro second
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rtk_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us)
{
    (void)base_addr;
    (void)timer_tick_us;
    watchdog_init(time_us/1000);
    return 0;
}

static uint32_t watchdog_runing_irq_handler(void *id)
{
	(void)id;
	watchdog_refresh();

	/*WDG_ClearINT was included in watchdog_irq_init*/
    watchdog_irq_init(watchdog_runing_irq_handler, 0);

	return 0;
}

/**
    @brief           - Enables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rtk_enable(addr_t base_addr)
{
    /* Enable counter by enabling intr and reset */
    (void)base_addr;
    watchdog_start();

	/*TFM API Test will reEnter pal_wd_rtk_enable many times*/
    watchdog_irq_init(watchdog_runing_irq_handler, 0);
    return 0;
}

static uint32_t watchdog_stop_irq_handler(void *id)
{
	(void)id;

	watchdog_refresh();

	/*WDG_ClearINT was included in watchdog_irq_init*/
    watchdog_irq_init(watchdog_stop_irq_handler, 0);

	return 0;
}

/**
    @brief           - Disables a hardware watchdog timer
    @param           - base_addr       : Base address of the watchdog module
    @return          - SUCCESS/FAILURE
**/
int pal_wd_rtk_disable(addr_t base_addr)
{
    /* RTK WDG cannot be disabled, refresh it here. */
    (void)base_addr;
	watchdog_refresh();

	/*Dplus WDG Cannot Disabled, Use WDG INT to refresh forever*/
    watchdog_irq_init(watchdog_stop_irq_handler, 0);
    return 0;
}

