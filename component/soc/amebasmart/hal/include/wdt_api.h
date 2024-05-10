/** mbed Microcontroller Library
  ******************************************************************************
  * @file    wdt_api.h
  * @author
  * @version V1.0.0
  * @brief   This file provides following mbed WDT API
  ******************************************************************************
  * @attention
  *
  * Copyright(c) 2006 - 2022 Realtek Corporation. All rights reserved.
  *
  * SPDX-License-Identifier: Apache-2.0
  *
  * Licensed under the Apache License, Version 2.0 (the License); you may
  * not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.apache.org/licenses/LICENSE-2.0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an AS IS BASIS, WITHOUT
  * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
#ifndef MBED_WATCHDOG_API_H
#define MBED_WATCHDOG_API_H

#include "device.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef u32(*wdt_irq_handler)(void *id);

void watchdog_init(uint32_t timeout_ms);
void watchdog_start(void);
void watchdog_stop(void);
void watchdog_refresh(void);
void watchdog_irq_init(wdt_irq_handler handler, uint32_t id);


#ifdef __cplusplus
}
#endif

#endif
