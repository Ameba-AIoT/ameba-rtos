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
#ifndef _S001_TEST_DATA_H_
#define _S001_TEST_DATA_H_

#include "test_s001.h"

static const test_data_t s001_data[] = {
{
    VAL_TEST_IDX0, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Call the get API when no UID is set */
    VAL_TEST_IDX1, {VAL_ITS_GET, VAL_PS_GET}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call the get_info API when no UID is set */
    VAL_TEST_IDX2, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call the remove API when no UID is set */
    VAL_TEST_IDX3, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Create a valid storage entity with UID1 */
    VAL_TEST_IDX4, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Create a valid storage entity with UID2 */
    VAL_TEST_IDX5, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Remove UID1 */
    VAL_TEST_IDX6, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_SUCCESS
},
{
    /* Call get API for UID1 */
    VAL_TEST_IDX7, {VAL_ITS_GET, VAL_PS_GET}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call get_info API for UID1 */
    VAL_TEST_IDX8, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call remove API for UID1 */
    VAL_TEST_IDX9, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Create a valid storage entity again with UID1 */
    VAL_TEST_IDX10, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Call get API for UID not same as UID1 or UID2 */
    VAL_TEST_IDX11, {VAL_ITS_GET, VAL_PS_GET}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call get_info for UID not same as UID1 or UID2 */
    VAL_TEST_IDX12, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Call remove API for UID not same as UID1 or UID2 */
    VAL_TEST_IDX13, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_DOES_NOT_EXIST
},
{
    /* Remove UID1 */
    VAL_TEST_IDX14, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_SUCCESS
},
{
    /* Remove UID2 */
    VAL_TEST_IDX15, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_SUCCESS
},
};
#endif /* _S001_TEST_DATA_H_ */

