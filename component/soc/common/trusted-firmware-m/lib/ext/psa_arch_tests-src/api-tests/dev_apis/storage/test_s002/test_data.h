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
#ifndef _S002_TEST_DATA_H_
#define _S002_TEST_DATA_H_

#include "test_s002.h"

static struct psa_storage_info_t orig_info;
static struct psa_storage_info_t new_info;
static const test_data_t s002_data[] = {
{
    VAL_TEST_IDX0, {VAL_API_UNUSED, VAL_API_UNUSED},  0
},
{
    /* Create a valid storage with create flag value 0 */
    VAL_TEST_IDX1, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Call the get_info API to validate the attributes */
    VAL_TEST_IDX2, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_SUCCESS
},
{
    VAL_TEST_IDX3, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    VAL_TEST_IDX4, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Validate the data using get API */
    VAL_TEST_IDX5, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX6, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Change the flag to WRITE_ONCE using set API */
    VAL_TEST_IDX7, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Call the get_info API to validate the flag change */
    VAL_TEST_IDX8, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_SUCCESS
},
{
    VAL_TEST_IDX9, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    VAL_TEST_IDX10, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Validate the data using get API after flag change */
    VAL_TEST_IDX11, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX12, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Storage should not be removed after WRITE_ONCE flag */
    VAL_TEST_IDX13, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Create a storage with different UID and flag value WRITE_ONCE */
    VAL_TEST_IDX14, {VAL_ITS_SET, VAL_PS_SET}, PSA_SUCCESS
},
{
    /* Storage should not be removed */
    VAL_TEST_IDX15, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Validate the data using get API after flag change */
    VAL_TEST_IDX16, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX17, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Call the get_info API to validate the flag change */
    VAL_TEST_IDX18, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_SUCCESS
},
{
    VAL_TEST_IDX19, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    VAL_TEST_IDX20, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Try to set different size for same UID and flag value */
    VAL_TEST_IDX21, {VAL_ITS_SET, VAL_PS_SET}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Storage should not be removed */
    VAL_TEST_IDX22, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Call the get_info API to validate the flag change */
    VAL_TEST_IDX23, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_SUCCESS
},
{
    VAL_TEST_IDX24, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    VAL_TEST_IDX25, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Validate the data using get API after flag change */
    VAL_TEST_IDX26, {VAL_ITS_GET, VAL_PS_GET}, PSA_SUCCESS
},
{
    VAL_TEST_IDX27, {VAL_API_UNUSED, VAL_API_UNUSED}, 0
},
{
    /* Setting flag to zero for UID should fail */
    VAL_TEST_IDX28, {VAL_ITS_SET, VAL_PS_SET}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Storage should not be removed */
    VAL_TEST_IDX29, {VAL_ITS_REMOVE, VAL_PS_REMOVE}, PSA_ERROR_NOT_PERMITTED
},
{
    /* Check that the WRITE_ONCE flag is preserved */
    VAL_TEST_IDX30, {VAL_ITS_GET_INFO, VAL_PS_GET_INFO}, PSA_SUCCESS
},
};
#endif /* _S002_TEST_DATA_H_ */
