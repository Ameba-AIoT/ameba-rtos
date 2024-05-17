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


#ifndef _VAL_INFRA_H_
#define _VAL_INFRA_H_

#include "val.h"
#include "val_client_defs.h"
#include "val_interfaces.h"

/* prototypes */
uint32_t     val_report_status(void);
val_status_t val_set_status(uint32_t status);
uint32_t     val_get_status(void);
val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status);
void         val_test_init(uint32_t test_num, char8_t *desc, uint32_t test_bitfield);
void         val_test_exit(void);
val_status_t val_get_last_run_test_id(test_id_t *test_id);
val_status_t val_execute_non_secure_tests(uint32_t test_num,
                                          const client_test_t *tests_list,
                                          bool_t server_hs);
val_status_t val_switch_to_secure_client(uint32_t test_num);
val_status_t val_execute_secure_test_func(psa_handle_t *handle,
                                          test_info_t test_info,
                                          uint32_t sid);
val_status_t val_get_secure_test_result(psa_handle_t *handle);
val_status_t val_ipc_connect(uint32_t sid, uint32_t version, psa_handle_t *handle);
val_status_t val_ipc_call(psa_handle_t handle,
                          int32_t type,
                          const psa_invec *in_vec,
                          size_t in_len,
                          psa_outvec *out_vec,
                          size_t out_len);
void         val_ipc_close(psa_handle_t handle);
val_status_t val_set_boot_flag(boot_state_t state);
val_status_t val_get_boot_flag(boot_state_t *state);
val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data);
val_status_t val_get_test_data(int32_t nvm_index, int32_t *test_data);
#endif
