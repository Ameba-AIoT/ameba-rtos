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
#ifndef _S010_TEST_DATA_H_
#define _S010_TEST_DATA_H_

#include "test_s010.h"

static const test_data_t s010_data[] = {
{
    /* Create with UID value zero should fail */
    VAL_TEST_IDX0, {VAL_ITS_SET, VAL_PS_SET}, PSA_ERROR_INVALID_ARGUMENT
},
{
    /* Call to get_info API for UID 0 should fail */
    VAL_TEST_IDX1, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_ERROR_INVALID_ARGUMENT
},
{
    /* Call to remove API UID value zero should fail */
    VAL_TEST_IDX2, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_INVALID_ARGUMENT
},
};
#endif /* _S010_TEST_DATA_H_ */
