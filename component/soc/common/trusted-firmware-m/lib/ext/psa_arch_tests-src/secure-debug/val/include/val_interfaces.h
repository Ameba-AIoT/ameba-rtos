/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_INTERFACES_H_
#define _VAL_INTERFACES_H_

#include "val.h"

/* typedef's */
typedef struct {
    val_status_t     (*print)                     (print_verbosity_t verbosity,
                                                   const char *string, int32_t data);
    val_status_t     (*set_status)                (uint32_t status);
    uint32_t         (*get_status)                (void);
    void             (*test_init)                 (uint32_t test_num,  char8_t *desc);
    void             (*test_exit)                 (void);
    val_status_t     (*err_check_set)             (uint32_t checkpoint, val_status_t status);
} val_api_t;

typedef void (*test_fptr_t)(val_api_t *val);

typedef struct {
    test_id_t     test_id;
    test_fptr_t   entry_addr;
} val_test_info_t;

typedef enum {
    VAL_TEST_IDX0    = 0x0,
    VAL_TEST_IDX1    = 0x1,
    VAL_TEST_IDX2    = 0x2,
    VAL_TEST_IDX3    = 0x3,
    VAL_TEST_IDX4    = 0x4,
    VAL_TEST_IDX5    = 0x5,
    VAL_TEST_IDX6    = 0x6,
    VAL_TEST_IDX7    = 0x7,
    VAL_TEST_IDX8    = 0x8,
    VAL_TEST_IDX9    = 0x9,
    VAL_TEST_IDX10   = 0xA,
    VAL_TEST_IDX11   = 0xB,
    VAL_TEST_IDX12   = 0xC,
    VAL_TEST_IDX13   = 0xD,
    VAL_TEST_IDX14   = 0xE,
    VAL_TEST_IDX15   = 0xF,
    VAL_TEST_IDX16   = 0x10,
    VAL_TEST_IDX17   = 0x11,
    VAL_TEST_IDX18   = 0x12,
    VAL_TEST_IDX19   = 0x13,
    VAL_TEST_IDX20   = 0x14,
    VAL_TEST_IDX21   = 0x15,
    VAL_TEST_IDX22   = 0x16,
    VAL_TEST_IDX23   = 0x17,
    VAL_TEST_IDX24   = 0x18,
    VAL_TEST_IDX25   = 0x19,
    VAL_TEST_IDX26   = 0x1A,
    VAL_TEST_IDX27   = 0x1B,
    VAL_TEST_IDX28   = 0x1C,
    VAL_TEST_IDX29   = 0x1D,
    VAL_TEST_IDX30   = 0x1E,
} val_test_index_t;

#include "test_entry_fn_declare_list.inc"

void test_entry(val_api_t *val);
#endif
