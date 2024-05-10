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

#ifndef _VAL_PSA_SERVICE_H_
#define _VAL_PSA_SERVICE_H_

/***************** PSA Secure Function API *****************/

/* psa/service.h: Secure Partition API elements. Only accessible to Secure Partition */
#include "psa/service.h"

/* psa/lifecycle.h: Contains the PSA Lifecycle API elements */
#include "psa/lifecycle.h"

/* "psa_manifest/<manifestfilename>.h" Manifest definitions. Only accessible to Secure Partition.
 * The file name is based on the name of the Secure Partitions manifest file.
 * The name must not collide with other header files.
 * Compliance tests expect the below manifest output files implementation from build tool.
 */
#include "psa_manifest/driver_partition_psa.h"
#ifndef DRIVER_PARTITION_INCLUDE
#include "psa_manifest/client_partition_psa.h"
#include "psa_manifest/server_partition_psa.h"
#endif

#include "val_target.h"

/* struct of function pointers to uniqify  nspe and spe client interface for test */
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
    psa_signal_t     (*wait)                  (psa_signal_t signal_mask, uint32_t timeout);
    void             (*set_rhandle)           (psa_handle_t msg_handle, void *rhandle);
    psa_status_t     (*get)                   (psa_signal_t signal, psa_msg_t *msg);
    size_t           (*read)                  (psa_handle_t msg_handle, uint32_t invec_idx,
                                               void *buffer, size_t num_bytes);
    size_t           (*skip)                  (psa_handle_t msg_handle, uint32_t invec_idx,
                                               size_t num_bytes);
    void             (*write)                 (psa_handle_t msg_handle, uint32_t outvec_idx,
                                               const void *buffer, size_t num_bytes);
    void             (*reply)                 (psa_handle_t msg_handle, psa_status_t status);
    void             (*notify)                (int32_t partition_id);
    void             (*clear)                 (void);
    void             (*eoi)                   (psa_signal_t irq_signal);
    uint32_t         (*rot_lifecycle_state)   (void);
    void             (*panic)                 (void);
} psa_api_t;

typedef struct {
  val_status_t (*print)                      (print_verbosity_t verbosity,
                                              char *string, int32_t data);
  val_status_t (*err_check_set)              (uint32_t checkpoint, val_status_t status);
  val_status_t (*execute_secure_test_func)   (psa_handle_t *handle, test_info_t test_info,
                                              uint32_t sid);
  val_status_t (*get_secure_test_result)     (psa_handle_t *handle);
  val_status_t (*ipc_connect)                (uint32_t sid, uint32_t version,
                                              psa_handle_t *handle );
  val_status_t (*ipc_call)                   (psa_handle_t handle,
                                              int32_t type,
                                              const psa_invec *in_vec,
                                              size_t in_len,
                                              psa_outvec *out_vec,
                                              size_t out_len);
  void         (*ipc_close)                  (psa_handle_t handle);
  val_status_t (*set_boot_flag)              (boot_state_t state);
  val_status_t (*set_test_data)             (int32_t nvm_index, int32_t test_data);
  val_status_t (*target_get_config)          (cfg_id_t cfg_id, uint8_t **data, uint32_t *size);
  val_status_t (*process_connect_request)    (psa_signal_t sig, psa_msg_t *msg);
  val_status_t (*process_call_request)       (psa_signal_t sig, psa_msg_t *msg);
  val_status_t (*process_disconnect_request) (psa_signal_t sig, psa_msg_t *msg);
} val_api_t;
#endif
