/** @file
 * Copyright (c) 2019, Arm Limited or its affiliates. All rights reserved.
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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 8),
                  CREATE_TEST_TITLE("Check from default T target for IRQ originating from TW operation"),
                  CREATE_REF_TAG("R120_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t        *g_val;
soc_peripheral_hdr_t  *per_hdr;
soc_peripheral_desc_t *per_desc;

void trusted_target (void)
{
    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

    g_val->print(PRINT_DEBUG, "\n\r\tIn Trusted target for IRQ 0x%X", per_desc->intr_id);
}

void entry_hook(tbsa_val_api_t *val)
{
    g_val = val;

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      instance;
    uint32_t      per_num;
    uint32_t      timeout      = TIMEOUT;
    uint32_t      minor_id     = 0x1;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&per_hdr,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    for(per_num = 0; per_num < per_hdr->num;) {
        instance = 0;
        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, minor_id, instance),
                                            (uint8_t **)&per_desc,
                                            (uint32_t *)sizeof(soc_peripheral_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
                return;
            }

            if (per_desc->attribute == SECURE_PROGRAMMABLE) {

                /* Installing Trusted Handler */
                status = val->interrupt_setup_handler(EXCP_NUM_EXT_INT(per_desc->intr_id), 0, trusted_target);
                if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                    return;
                }

                /* Write control bit */
                val->set_status(RESULT_PENDING(status));

                /* SET pending bit for a given trusted timer IRQ(trusted mode) */
                status = val->interrupt_set_pending(EXCP_NUM_EXT_INT(per_desc->intr_id));
                if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                    return;
                }

                /* stay here till trsuted target entered or timeout occurs, whichever comes early */
                while (IS_TEST_PENDING(val->get_status()) && (--timeout));

                /* Restoring default Handler */
                status = val->interrupt_restore_handler(EXCP_NUM_EXT_INT(per_desc->intr_id));
                if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                    return;
                }

                if(!timeout) {
                    val->print(PRINT_ERROR, "\n\r\tInterrupt did not reach Trusted target in its default state for IRQ : 0x%X", per_desc->intr_id);
                    val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_TIMEOUT);
                    return;
                }

                timeout = TIMEOUT;
            }

            instance++;
        } while (instance < GET_NUM_INSTANCE(per_desc));
        minor_id++;
        if (status != TBSA_STATUS_NOT_FOUND) {
            per_num += GET_NUM_INSTANCE(per_desc);
        }
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
