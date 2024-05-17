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

#ifndef _PAL_DRIVER_INTF_H_
#define _PAL_DRIVER_INTF_H_

#include "pal_uart.h"
#include "pal_nvmem.h"
#include "pal_wd_cmsdk.h"

void pal_uart_init(uint32_t uart_base_addr);
void pal_print(const char *str, int32_t data);
int pal_nvmem_write(addr_t base, uint32_t offset, void *buffer, int size);
int pal_nvmem_read(addr_t base, uint32_t offset, void *buffer, int size);
int pal_wd_timer_init(addr_t base_addr, uint32_t time_us, uint32_t timer_tick_us);
int pal_wd_timer_enable(addr_t base_addr);
int pal_wd_timer_disable(addr_t base_addr);
int pal_wd_timer_is_enabled(addr_t base_addr);
void pal_generate_interrupt(void);
void pal_disable_interrupt(void);
#endif /* _PAL_DRIVER_INTF_H_ */
