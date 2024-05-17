/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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
#ifndef _TEST_A003_TESTS_H_
#define _TEST_A003_TESTS_H_

#define test_entry CONCAT(test_entry_, a003)

extern val_api_t *val;
extern char *key_file;
extern char *chain_file;

static val_status_t check_lifecycle_major_state(uint8_t *value, size_t size);
static val_status_t check_adac_token_support(uint8_t *value, size_t size);
static val_status_t check_adac_cert_support(uint8_t *value, size_t size);

#endif /* _TEST_A003_TESTS_H_ */
