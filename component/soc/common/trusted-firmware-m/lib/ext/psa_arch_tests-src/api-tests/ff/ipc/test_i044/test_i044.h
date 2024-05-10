/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _TEST_I044_CLIENT_TESTS_H_
#define _TEST_I044_CLIENT_TESTS_H_

#include "val_client_defs.h"

#ifdef NONSECURE_TEST_BUILD
#define test_entry CONCAT(test_entry_, i044)
#define val CONCAT(val, test_entry)
#define psa CONCAT(psa, test_entry)
#else
#define val CONCAT(val, _client_sp)
#define psa CONCAT(psa, _client_sp)
#endif

extern val_api_t *val;
extern psa_api_t *psa;

extern const client_test_t test_i044_client_tests_list[];

int32_t client_test_psa_write_with_invec_equal_to_max_iovec(caller_security_t);
#endif
