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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 6),
                  CREATE_TEST_TITLE("Check programmable address remapping logic configuration from the TW."),
                  CREATE_REF_TAG("R040_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_hdr_t  *soc_peripheral_hdr;
soc_peripheral_desc_t *soc_peripheral_desc;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data, shcsr = 0, index = 0;
    bool_t        intrcnt_remap_found = FALSE;
    uint32_t      intrcnt_num_inst = 0;
    uint32_t      timeout = TIMEOUT;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_peripheral_hdr,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        goto restore;
    }

    intrcnt_num_inst = GET_NUM_INSTANCE(soc_peripheral_hdr);
    while (index < intrcnt_num_inst) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_INTRCNT, index),
                                        (uint8_t **)&soc_peripheral_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (status != TBSA_STATUS_SUCCESS) {
            val->err_check_set(TEST_CHECKPOINT_4, status);
            break;
        }

        /* Incrementing the index to verify all instance */
        index++;
        intrcnt_remap_found = TRUE;

        val->set_status(RESULT_PENDING(status));

        /* Trying to read the remap logic register in the interconnect */
        val_mem_read_wide((uint32_t *)soc_peripheral_desc->base, &data);

        while (IS_TEST_PENDING(val->get_status()) && --timeout);

        if(!timeout) {
            val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing Interconnect 0x%X from NT world!", \
                                     (uint32_t)(soc_peripheral_desc->base));
            val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_TIMEOUT);
            return;
        }

        timeout = TIMEOUT;
    }

restore:
    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    if(!intrcnt_remap_found) {
        val->set_status(RESULT_SKIP(status));
    } else {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }
}
