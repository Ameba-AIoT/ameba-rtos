 /** @file
  *
  * Copyright (c) 2021 Nordic Semiconductor ASA.
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

#ifndef NRF_WDT_H__
#define NRF_WDT_H__

#include "pal_common.h"


int nrf_wdt_init(addr_t base_addr, uint32_t time_us);
int nrf_wdt_enable(addr_t base_addr);
int nrf_wdt_disable(addr_t base_addr);
int nrf_wdt_is_enabled(addr_t base_addr);

#endif // NRF_WDT_H__
