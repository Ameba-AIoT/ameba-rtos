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
#ifndef _S011_TEST_DATA_H_
#define _S011_TEST_DATA_H_

#include "test_s011.h"

static const test_data_t s011_data[] = {
{
    /* Check if optional PS API supported */
    VAL_TEST_IDX0, {VAL_API_UNUSED, VAL_PS_GET_SUPPORT}, PSA_STORAGE_SUPPORT_SET_EXTENDED
},
{
    /* Call set_extended call for non-existing UID */
    VAL_TEST_IDX1, {VAL_API_UNUSED, VAL_PS_SET_EXTENDED}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Create valid storage using set API */
    VAL_TEST_IDX2, {VAL_API_UNUSED, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Create API for present UID with different length */
    VAL_TEST_IDX3, {VAL_API_UNUSED, VAL_PS_CREATE}, PSA_ERROR_ALREADY_EXISTS
},
{
    /* Call create API for to set WRITE_ONCE flag */
    VAL_TEST_IDX4, {VAL_API_UNUSED, VAL_PS_CREATE}, PSA_ERROR_NOT_SUPPORTED
},
{
    /* Validate existing UID attributes maintained */
    VAL_TEST_IDX5, {VAL_API_UNUSED, VAL_PS_GET_INFO}, PSA_SUCCESS
},
{
    VAL_TEST_IDX6, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    VAL_TEST_IDX7, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Remove the UID */
    VAL_TEST_IDX8, {VAL_API_UNUSED, VAL_PS_REMOVE}, PSA_SUCCESS
},
{
    /* Create valid storage using create API */
    VAL_TEST_IDX9, {VAL_API_UNUSED, VAL_PS_CREATE}, PSA_SUCCESS
},
{
    /* Again call create API with different length */
    VAL_TEST_IDX10, {VAL_API_UNUSED, VAL_PS_CREATE}, PSA_ERROR_ALREADY_EXISTS
},
{
    /* Validate the storage is empty */
    VAL_TEST_IDX11, {VAL_API_UNUSED, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX12, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Remove the UID */
    VAL_TEST_IDX13, {VAL_API_UNUSED, VAL_PS_REMOVE}, PSA_SUCCESS
},
{
    /* Set_extended call for removed UID */
    VAL_TEST_IDX14, {VAL_API_UNUSED, VAL_PS_SET_EXTENDED}, PSA_ERROR_DOES_NOT_EXIST
}
};
#endif /* _S011_TEST_DATA_H_ */
