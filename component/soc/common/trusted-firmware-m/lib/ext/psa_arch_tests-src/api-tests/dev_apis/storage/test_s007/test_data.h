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
#ifndef _S007_TEST_DATA_H_
#define _S007_TEST_DATA_H_

#include "test_s007.h"

static const test_data_t s007_data[] = {
{
    VAL_TEST_IDX0, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Create a valid storage entity */
    VAL_TEST_IDX1, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Increase the length of storage */
    VAL_TEST_IDX2, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Try to access old length */
    VAL_TEST_IDX3, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    /* Try to access valid length less than set length */
    VAL_TEST_IDX4, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX5, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Decrease the length of storage */
    VAL_TEST_IDX6, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Try to access old length */
    VAL_TEST_IDX7, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    /* Try to access old length */
    VAL_TEST_IDX8, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    /* Try to access data with correct length */
    VAL_TEST_IDX9, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    /* Remove the storage entity */
    VAL_TEST_IDX10, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_SUCCESS
},
};
#endif /* _S007_TEST_DATA_H_ */
