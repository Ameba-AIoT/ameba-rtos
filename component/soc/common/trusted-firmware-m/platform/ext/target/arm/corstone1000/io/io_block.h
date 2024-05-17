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

#ifndef __IO_BLOCK_H__
#define __IO_BLOCK_H__

#include "io_storage.h"

/* block devices ops */
typedef struct io_block_ops {
    size_t (*read)(int lba, uintptr_t buf, size_t size);
    size_t (*write)(int lba, const uintptr_t buf, size_t size);
} io_block_ops_t;

typedef struct io_block_dev_spec {
    io_block_spec_t buffer;
    io_block_ops_t ops;
    size_t block_size;
} io_block_dev_spec_t;

struct io_dev_connector;

int register_io_dev_block(const struct io_dev_connector **dev_con);

#endif /* __IO_BLOCK_H__ */