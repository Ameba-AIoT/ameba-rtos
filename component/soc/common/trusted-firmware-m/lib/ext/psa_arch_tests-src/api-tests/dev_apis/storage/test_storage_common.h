/** @file
 * Copyright (c) 2020-2021, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _TEST_STORAGE_COMMON_H_
#define _TEST_STORAGE_COMMON_H_

#include "val_storage.h"

#define val              CONCAT(val, test_entry)
#define psa              CONCAT(psa, test_entry)
#define STORAGE_FUNCTION val->storage_function
#define ITS_TEST_MESSAGE "\n[Info] Executing ITS tests\n"
#define PS_TEST_MESSAGE  "\n[Info] Executing PS tests\n"
#define TEST_DESC_STORAGE " | UT: STORAGE\n"
#define TEST_DESC_ITS " | UT: ITS\n"
#define TEST_DESC_PS " | UT: PS\n"

extern val_api_t           *val;
extern psa_api_t           *psa;

typedef struct {
    val_test_index_t        idx;
    storage_function_type_t api[2];
    psa_status_t            status;
} test_data_t;

#endif /* _TEST_STORAGE_COMMON_H_ */
