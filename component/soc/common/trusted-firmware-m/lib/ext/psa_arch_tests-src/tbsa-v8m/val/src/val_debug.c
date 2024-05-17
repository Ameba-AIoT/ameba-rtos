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

#include "val_debug.h"
#include "val_target.h"
#include "val_infra.h"
#include "val_pe.h"

static uint32_t g_debug_test_start = 0;
/**
    @brief    - This function will get the status of the debugger,
                based on the access_type
    @param    - dbg_access - read/write/init
    @return   - error status
**/
tbsa_status_t val_debug_get_status(dbg_access_t dbg_access)
{
    tbsa_status_t status;
    dpm_desc_t    *dpm_desc;
    uint32_t      timeout = DBG_TIMEOUT;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, 0),
                                    (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
    if(status)
        return status;

    switch (dbg_access) {
        case DBG_INIT:
            if (!g_debug_test_start) {
                if (DBG_INIT_SIGNATURE != *(uint32_t *)dpm_desc->data_addr) {
                    val_print(PRINT_WARN, "\n\r\tSkipping the test as External Debugger is not present", 0);
                    val_print(PRINT_DEBUG, "\n\r\tData present in the handshake address 0x%x", *(uint32_t *)dpm_desc->data_addr);
                    return TBSA_STATUS_SKIP;
                }
                g_debug_test_start++;
                val_print(PRINT_INFO, "\n\r\tExternal Debugger initial handshake successful", 0);
            } else {
                if (DBG_NEXT_SIGNATURE != *(uint32_t *)dpm_desc->data_addr) {
                    val_print(PRINT_WARN, "\n\r\tSkipping the test as previous test corrupted handshake data", 0);
                    val_print(PRINT_DEBUG, "\n\r\tData present in the handshake address 0x%x", *(uint32_t *)dpm_desc->data_addr);
                    return TBSA_STATUS_SKIP;
                }
                val_print(PRINT_INFO, "\n\r\tExternal Debugger handshake successful", 0);
            }
            break;
        case DBG_WRITE:
            while (((DBG_FLAG_READY & *(uint32_t *)dpm_desc->flag_addr) != DBG_FLAG_READY) && (--timeout));
            if (!timeout) {
                val_print(PRINT_ERROR, "\n\r\tTimeout: External Debugger is not ready", 0);
                return TBSA_STATUS_TIMEOUT;
            }
            timeout = DBG_TIMEOUT;
            /* Poll for Txfull to be '0' before writing to the data register*/
            while (((DBG_FLAG_TXFULL & *(uint32_t *)dpm_desc->flag_addr) == DBG_FLAG_TXFULL) && (--timeout));
            if (!timeout) {
                val_print(PRINT_ERROR, "\n\r\tTimeout while polling for TxFull=0", 0);
                return TBSA_STATUS_TIMEOUT;
            }
            break;
        case DBG_READ:
            while (((DBG_FLAG_READY & *(uint32_t *)dpm_desc->flag_addr) != DBG_FLAG_READY) && (--timeout));
            if (!timeout) {
                val_print(PRINT_ERROR, "\n\r\tTimeout: External Debugger is not ready", 0);
                return TBSA_STATUS_TIMEOUT;
            }
            timeout = DBG_TIMEOUT;
            /* Poll for Rxfull to be '1' before reading the data updated by debugger*/
            while (((DBG_FLAG_RXFULL & *(uint32_t *)dpm_desc->flag_addr) != DBG_FLAG_RXFULL) && (--timeout));
            if (!timeout) {
                val_print(PRINT_ERROR, "\n\r\tTimeout while polling for RxFull=1", 0);
                return TBSA_STATUS_TIMEOUT;
            }
            break;
        default:
            return TBSA_STATUS_INCORRECT_VALUE;
    }

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will set the status of the debugger,
                based on the access_type
    @param    - dbg_access     : read/write/init
              - dbg_seq_status : this will choose the sequence to be executed on debugger
    @return   - error status
**/
tbsa_status_t val_debug_set_status(dbg_access_t dbg_access, dbg_seq_status_t dbg_status)
{
    tbsa_status_t status;
    dpm_desc_t    *dpm_desc;
    uint32_t      timeout = DBG_TIMEOUT;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, 0),
                                    (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
    if(status)
        return status;

    switch (dbg_access) {
        case DBG_INIT:
            while (((DBG_FLAG_READY & *(uint32_t *)dpm_desc->flag_addr) != DBG_FLAG_READY) && (--timeout));
            if (!timeout) {
                val_print(PRINT_ERROR, "\n\r\tTimeout: External Debugger is not ready", 0);
                return TBSA_STATUS_TIMEOUT;
            }
            *(uint32_t *)dpm_desc->flag_addr = dbg_status;
            __asm volatile("DSB");
            __asm volatile("ISB");
            break;
        case DBG_WRITE:
            *(uint32_t *)dpm_desc->flag_addr = dbg_status | DBG_FLAG_TXFULL;
            break;
        case DBG_READ:
            *(uint32_t *)dpm_desc->flag_addr &= ~DBG_FLAG_RXFULL;
            __asm volatile("DSB");
            __asm volatile("ISB");
            break;
        default:
            return TBSA_STATUS_INCORRECT_VALUE;
    }

    return TBSA_STATUS_SUCCESS;
}

/**
    @brief    - This function will set the DPM state
    @param    - dpm_desc       : dpm_desc_t
                dpm_status     : DPM status as per TBSA spec, defined in dpm_status_t
    @return   - error status
**/
tbsa_status_t val_dpm_set_state(dpm_desc_t *dpm_desc, dpm_status_t dpm_status)
{
    switch(dpm_desc->unlock_token)
    {
        case TOKEN_PASSWD:
            return pal_dpm_set_state(dpm_desc->index, dpm_status, dpm_desc->unlock_token, (void *)(dpm_desc->passwd));
        case TOKEN_CERTIFICATE:
            return pal_dpm_set_state(dpm_desc->index, dpm_status, dpm_desc->unlock_token, (void *)(dpm_desc->certificate_addr));
        default:
            return TBSA_STATUS_INVALID_ARGS;
    }
}

/**
    @brief    - This function will set the DPM state
    @param    - index          : DPM index
                dpm_status     : DPM status as per TBSA spec, can returned as logical OR of in relevant cases
                                 DPM_LOCK_IMPLEMENTED/DPM_LOCK_VALUE
                                 DPM_EN_IMPLEMENTED/DPM_EN_VALUE
    @return   - error status
**/
tbsa_status_t val_dpm_get_state(uint32_t index, uint32_t *dpm_status)
{
    return pal_dpm_get_state(index, dpm_status);
}

/**
    @brief    - This function will set the debug permission based on the input arg
    @param    - index          : DPM index
                access_ns      : TRUE  - allow debug access only for non-secure address
                                 FALSE - allow debug access to both secure and non-secure addresses
    @return   - error status
**/
tbsa_status_t val_dpm_set_access_ns_only(uint32_t index, bool_t access_ns)
{
    return pal_dpm_set_access_ns_only(index, access_ns);
}
