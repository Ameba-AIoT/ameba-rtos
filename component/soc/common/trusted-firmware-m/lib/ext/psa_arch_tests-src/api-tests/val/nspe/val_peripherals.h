/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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

#include "val.h"

val_status_t val_uart_init(void);
val_status_t val_print(print_verbosity_t verbosity, const char *string, int32_t data);
val_status_t val_nvmem_read(uint32_t offset, void *buffer, int size);
val_status_t val_nvmem_write(uint32_t offset, void *buffer, int size);
val_status_t val_wd_timer_init(wd_timeout_type_t timeout_type);
val_status_t val_wd_timer_enable(void);
val_status_t val_wd_timer_disable(void);
val_status_t val_wd_reprogram_timer(wd_timeout_type_t timeout_type);
#endif
