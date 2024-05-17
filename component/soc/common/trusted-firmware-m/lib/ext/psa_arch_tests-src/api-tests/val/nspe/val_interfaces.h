/** @file
 * Copyright (c) 2018-2022, Arm Limited or its affiliates. All rights reserved.
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

#ifndef _VAL_INTERFACES_H_
#define _VAL_INTERFACES_H_

#include "val.h"
#include "val_client_defs.h"
#include "pal_interfaces_ns.h"

/* typedef's */
typedef struct {
    val_status_t     (*print)                     (print_verbosity_t verbosity,
                                                   const char *string, int32_t data);
    val_status_t     (*set_status)                (uint32_t status);
    uint32_t         (*get_status)                (void);
    void             (*test_init)                 (uint32_t test_num,  char8_t *desc,
                                                   uint32_t test_bitfield);
    void             (*test_exit)                 (void);
    val_status_t     (*err_check_set)             (uint32_t checkpoint, val_status_t status);
    val_status_t     (*target_get_config)         (cfg_id_t cfg_id, uint8_t **data, uint32_t *size);
    val_status_t     (*execute_non_secure_tests)  (uint32_t test_num,
                                                   const client_test_t *tests_list,
                                                   bool_t server_hs);
    val_status_t     (*switch_to_secure_client)   (uint32_t test_num);
    val_status_t     (*execute_secure_test_func)  (psa_handle_t *handle, test_info_t test_info,
                                                   uint32_t sid);
    val_status_t     (*ipc_connect)               (uint32_t sid, uint32_t version,
                                                   psa_handle_t *handle );
    val_status_t     (*ipc_call)                  (psa_handle_t handle,
                                                   int32_t type,
                                                   const psa_invec *in_vec,
                                                   size_t in_len,
                                                   psa_outvec *out_vec,
                                                   size_t out_len);
    void             (*ipc_close)                 (psa_handle_t handle);
    val_status_t     (*get_secure_test_result)    (psa_handle_t *handle);
    val_status_t     (*nvmem_read)                (uint32_t offset, void *buffer, int size);
    val_status_t     (*nvmem_write)               (uint32_t offset, void *buffer, int size);
    val_status_t     (*wd_timer_init)             (wd_timeout_type_t timeout_type);
    val_status_t     (*wd_timer_enable)           (void);
    val_status_t     (*wd_timer_disable)          (void);
    val_status_t     (*wd_reprogram_timer)        (wd_timeout_type_t timeout_type);
    val_status_t     (*set_boot_flag)             (boot_state_t state);
    val_status_t     (*get_boot_flag)             (boot_state_t *state);
    val_status_t     (*get_test_data)             (int32_t nvm_index, int32_t *test_data);
    val_status_t     (*set_test_data)             (int32_t nvm_index, int32_t test_data);
    int32_t          (*crypto_function)           (int type, ...);
    int32_t          (*storage_function)          (int type, ...);
    int32_t          (*attestation_function)      (int type, ...);
} val_api_t;

typedef struct {
    uint32_t         (*framework_version)     (void);
    uint32_t         (*version)               (uint32_t sid);
    psa_handle_t     (*connect)               (uint32_t sid, uint32_t version);
    psa_status_t     (*call)                  (psa_handle_t handle,
                                               int32_t type,
                                               const psa_invec *in_vec,
                                               size_t in_len,
                                               psa_outvec *out_vec,
                                               size_t out_len);
    void             (*close)                 (psa_handle_t handle);
} psa_api_t;

typedef void (*test_fptr_t)(val_api_t *val, psa_api_t *psa);

typedef struct {
    test_id_t     test_id;
    test_fptr_t   entry_addr;
} val_test_info_t;

typedef enum {
    VAL_TEST_IDX0    = 0x0,
    VAL_TEST_IDX1    = 0x1,
    VAL_TEST_IDX2    = 0x2,
    VAL_TEST_IDX3    = 0x3,
    VAL_TEST_IDX4    = 0x4,
    VAL_TEST_IDX5    = 0x5,
    VAL_TEST_IDX6    = 0x6,
    VAL_TEST_IDX7    = 0x7,
    VAL_TEST_IDX8    = 0x8,
    VAL_TEST_IDX9    = 0x9,
    VAL_TEST_IDX10   = 0xA,
    VAL_TEST_IDX11   = 0xB,
    VAL_TEST_IDX12   = 0xC,
    VAL_TEST_IDX13   = 0xD,
    VAL_TEST_IDX14   = 0xE,
    VAL_TEST_IDX15   = 0xF,
    VAL_TEST_IDX16   = 0x10,
    VAL_TEST_IDX17   = 0x11,
    VAL_TEST_IDX18   = 0x12,
    VAL_TEST_IDX19   = 0x13,
    VAL_TEST_IDX20   = 0x14,
    VAL_TEST_IDX21   = 0x15,
    VAL_TEST_IDX22   = 0x16,
    VAL_TEST_IDX23   = 0x17,
    VAL_TEST_IDX24   = 0x18,
    VAL_TEST_IDX25   = 0x19,
    VAL_TEST_IDX26   = 0x1A,
    VAL_TEST_IDX27   = 0x1B,
    VAL_TEST_IDX28   = 0x1C,
    VAL_TEST_IDX29   = 0x1D,
    VAL_TEST_IDX30   = 0x1E,
} val_test_index_t;

#include "test_entry_fn_declare_list.inc"

void test_entry(val_api_t *val, psa_api_t *psa);
#endif
