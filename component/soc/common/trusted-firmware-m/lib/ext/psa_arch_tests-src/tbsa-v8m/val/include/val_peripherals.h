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


#ifndef _VAL_PERIPHERALS_H_
#define _VAL_PERIPHERALS_H_

#include "val_common.h"
#include "val_target.h"
#include "val_infra.h"

tbsa_status_t val_i2c_read         (addr_t addr, uint8_t *data, uint32_t len);
tbsa_status_t val_i2c_write        (addr_t addr, uint8_t *data, uint32_t len);

tbsa_status_t val_spi_init  (void);
tbsa_status_t val_spi_read  (addr_t addr, uint8_t *data, uint32_t len);
tbsa_status_t val_spi_write (addr_t addr, uint8_t *data, uint32_t len);

tbsa_status_t val_timer_init           (addr_t addr, uint32_t time_us, uint32_t timer_tick_us);
tbsa_status_t val_timer_enable         (addr_t addr);
tbsa_status_t val_timer_disable        (addr_t addr);
tbsa_status_t val_timer_interrupt_clear(addr_t addr);

tbsa_status_t val_wd_timer_init       (addr_t addr, uint32_t time_us, uint32_t timer_tick_us);
tbsa_status_t val_wd_timer_enable     (addr_t addr);
tbsa_status_t val_wd_timer_disable    (addr_t addr);
tbsa_status_t val_is_wd_timer_enabled (addr_t addr);

tbsa_status_t val_mpc_configure_security_attribute (addr_t mpc, addr_t start_addr,addr_t end_addr, mem_tgt_attr_t sec_attr);

tbsa_status_t val_uart_init(addr_t addr);
tbsa_status_t val_uart_tx  (addr_t addr, const void *data, uint32_t num);

tbsa_status_t  val_nvram_read   (addr_t base, uint32_t offset, void *buffer, int size);
tbsa_status_t  val_nvram_write  (addr_t base, uint32_t offset, void *buffer, int size);

tbsa_status_t val_rtc_init                (void);
bool_t        val_is_rtc_trustable        (addr_t addr);
bool_t        val_is_rtc_synced_to_server (addr_t addr);

#endif /* _VAL_PERIPHERALS_H_ */
