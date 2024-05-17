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

#ifndef _PAL_WD_RTK_H_
#define _PAL_WD_RTK_H_

#include "pal_common.h"

#define WDOG_TIMER_MAX_VALUE           0xFFFFFFFF

typedef uint32_t (*wdt_irq_handler)(void *id);

/**
 * @brief   Initializes the watch dog, include time setting, mode register
 * @param   timeout_ms: the watch-dog timer timeout value, in ms.
 *           default action of timeout is to reset the whole system.
 * @retval none
 */
void watchdog_init(uint32_t timeout_ms);

/**
 * @brief  Start the watchdog counting
 * @param  None
 * @retval none
 */
void watchdog_start(void);

/**
 * @brief  Stop the watchdog counting
 * @param  None
 * @retval none
 */
void watchdog_stop(void);

/**
 * @brief  Refresh the watchdog counting to prevent WDT timeout
 * @param  None
 * @retval none
 */
void watchdog_refresh(void);

/**
 * @brief  Enable eraly interrupt and register a watchdog timer timeout interrupt handler.
 *        The interrupt handler will be called at a programmable time prior to watchdog timeout, for users to prepare for reset
 * @param  handler: WDT timeout interrupt callback function.
 * @param  id: WDT timeout interrupt callback parameter.
 * @retval none
 */
void watchdog_irq_init(wdt_irq_handler handler, uint32_t id);

int pal_wd_rtk_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int pal_wd_rtk_enable(addr_t base_addr);
int pal_wd_rtk_disable(addr_t base_addr);

#endif /* PAL_WD_RTK_H */
