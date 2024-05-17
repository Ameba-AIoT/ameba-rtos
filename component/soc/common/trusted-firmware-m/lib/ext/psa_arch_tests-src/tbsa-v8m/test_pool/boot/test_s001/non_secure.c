/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_BOOT_BASE, 1),
                  CREATE_TEST_TITLE("Trusted boot operation from Trusted and Non-trusted world"),
                  CREATE_REF_TAG("R010/R020/R030/R090_TBSA_BOOT"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t *memory_desc;

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status = TBSA_STATUS_SUCCESS;
    boot_t        boot;
    uint32_t      shcsr  = 0UL;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    if (boot.wb != WARM_BOOT_REQUESTED) {
        /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
        status = val->mem_reg_read(SHCSR, &shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }

        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }

        status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        boot.wb = WARM_BOOT_REQUESTED;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        /* Trying to get the last reset type.
         * Executing in non-secure mode, expecting secure fault
         */
        val_system_reset_type(WARM_RESET);
        /* Shouldn't come here */
        val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
        return;
    } else {

        /* If we are here means, we are in second run of this test */
        boot.wb = BOOT_UNKNOWN;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
            return;
        }

        status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
            return;
        }

        /* Restoring faults */
        status = val->mem_reg_write(SHCSR, shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
            return;
        }
    }

    /* See whether VTOR is relocated from ROM before TBSA does */
    if(val->is_vtor_relocated_from_rom()) {
        val->print(PRINT_WARN, "\n\tPrimary processor possibly booted from ROM;", 0);
        val->print(PRINT_WARN, "\n\tVTOR updated when control reached tbsa_entry", 0);
        val->set_status(RESULT_SKIP(TBSA_STATUS_UNKNOWN));
    } else {
        val->print(PRINT_DEBUG, "\n\tPrimary processor booted from ROM", 0);
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return;
    }
}
