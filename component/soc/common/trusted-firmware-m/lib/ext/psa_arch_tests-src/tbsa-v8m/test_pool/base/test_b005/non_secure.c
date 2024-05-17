/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_test_common.h"

#define TIMEOUT (0xFFFFFFFFUL)

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 5),
                  CREATE_TEST_TITLE("Check transaction filters config space must only be accessed from the TW"),
                  CREATE_REF_TAG("R060/R080/R210_TBSA_INFRA-R110_TBSA_FUSE"),
                  entry_hook,
                  test_payload,
                  exit_hook);

protection_units_desc_t *prot_units_desc;
uint32_t                 shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data;
    uint32_t      ppc_instance    = 0UL;
    uint32_t      mpc_instance    = 0UL;
    uint32_t      timeout         = TIMEOUT;
    bool_t        ppc_block_found = FALSE;
    bool_t        mpc_block_found = FALSE;

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, PROTECTION_UNITS_PPC, ppc_instance),
                                        (uint8_t **)&prot_units_desc,
                                        (uint32_t *)sizeof(protection_units_desc_t));
        if (status == TBSA_STATUS_NOT_FOUND) {
            break;
        }

        ppc_block_found = TRUE;

        /* Incrementing the instance value as the next call is expected to trigger fault */
        ppc_instance++;

        val->set_status(RESULT_PENDING(status));

        /* Trying to read the device base address of a given PPC, expect fault? */
        val_mem_read_wide((uint32_t *)prot_units_desc->device_base, &data);

        while (IS_TEST_PENDING(val->get_status()) && --timeout);

        if(!timeout) {
            val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing PPC 0x%X from NT world!", (uint32_t)(prot_units_desc->device_base));
            val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_TIMEOUT);
            return;
        }

        timeout = TIMEOUT;
    } while(TRUE);

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, PROTECTION_UNITS_MPC, mpc_instance),
                                        (uint8_t **)&prot_units_desc,
                                        (uint32_t *)sizeof(protection_units_desc_t));
        if (status == TBSA_STATUS_NOT_FOUND) {
            break;
        }

        mpc_block_found = TRUE;

        /* Incrementing the instance value as the next call is expected to trigger fault */
        mpc_instance++;

        val->set_status(RESULT_PENDING(status));

        /* Trying to read the device base address of a given MPC, expect fault? */
        val_mem_read_wide((uint32_t *)prot_units_desc->device_base, &data);

        while (IS_TEST_PENDING(val->get_status()) && --timeout);

        if(!timeout) {
            val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing MPC 0x%X from NT world!", (uint32_t)(prot_units_desc->device_base));
            val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_TIMEOUT);
            return;
        }

        timeout = TIMEOUT;
    } while(TRUE);

    if (!ppc_block_found && !mpc_block_found) {
        val->set_status(RESULT_SKIP(status));
    } else {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }
}
