/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _S015_TEST_DATA_H_
#define _S015_TEST_DATA_H_

#include "test_s015.h"

static const test_data_t s015_data[] = {
{
    /* Check if optional PS API supported */
    VAL_TEST_IDX0, {VAL_API_UNUSED, VAL_PS_GET_SUPPORT}, PSA_STORAGE_SUPPORT_SET_EXTENDED
},
{
    /* Call create API with write_once flag */
    VAL_TEST_IDX1, {VAL_API_UNUSED, VAL_PS_CREATE}, PSA_ERROR_NOT_SUPPORTED
},
};
#endif /* _S015_TEST_DATA_H_ */
