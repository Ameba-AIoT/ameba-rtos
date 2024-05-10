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

#define TIMEOUT_VALUE    0xFFFFFFFF

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_SECURE_RAM_BASE, 1),
                  CREATE_TEST_TITLE("Secure RAM access from Trusted world only"),
                  CREATE_REF_TAG("R160/R180_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;
uint32_t        shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__,
                            };

    val->test_initialize(&init);

    g_val = val;

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t                data         = 0;
    uint32_t                instance     = 0;
    protection_units_desc_t *prot_unit_desc;
    tbsa_status_t           status;
    uint32_t                timeout      = TIMEOUT_VALUE;
    bool_t                  mpc_found    = FALSE;

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_PROTECTION_UNITS, PROTECTION_UNITS_MPC, instance),
                                        (uint8_t **)&prot_unit_desc, (uint32_t *)sizeof(protection_units_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status))
            return;

        if (prot_unit_desc->attribute == SECURE_PROGRAMMABLE) {

            mpc_found = TRUE;

            val->set_status(RESULT_PENDING(status));

            /* Try to access MPC from Non-Secure */
            val_mem_read_wide((uint32_t *)prot_unit_desc->device_base, &data);

            /* Wait here till pending status is cleared by secure fault or timeout occurs, whichever comes early */
            while (IS_TEST_PENDING(val->get_status()) && (--timeout));

            if(!timeout) {
                val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing MPC 0x%X from NT world!", (uint32_t)(prot_unit_desc->device_base));
                val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_TIMEOUT);
                return;
            }
        }
        instance++;
        timeout = TIMEOUT_VALUE;
    } while(instance < GET_NUM_INSTANCE(prot_unit_desc));

    if (mpc_found) {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_NOT_FOUND);
        return;
    }
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }
}
