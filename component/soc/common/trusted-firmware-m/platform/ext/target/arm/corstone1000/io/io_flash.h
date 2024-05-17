/*
 * Copyright (c) 2022 Arm Limited. All rights reserved.
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
 */

#ifndef __IO_FLASH_H__
#define __IO_FLASH_H__

#include "io_storage.h"

typedef struct io_flash_dev_spec {
    uintptr_t buffer;
    size_t bufferlen;
    uint32_t base_addr;
    uintptr_t flash_driver;
} io_flash_dev_spec_t;

struct io_dev_connector;

/* Register the flash driver with the IO abstraction internally it register a
 * block device*/
int register_io_dev_flash(const struct io_dev_connector **dev_con);

#endif /* __IO_FLASH_H__ */
