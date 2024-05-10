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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_INTERRUPTS_BASE, 4),
                  CREATE_TEST_TITLE("Secure exception irqs must be wired/configured as Secure interrupt sources"),
                  CREATE_REF_TAG("R070_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t         status;
    soc_peripheral_hdr_t  *soc_peripheral_hdr;
    soc_peripheral_desc_t *soc_peripheral_desc;
    bool_t                source_to_check      = FALSE;
    uint32_t              per_num;
    uint32_t              minor_id;
    uint32_t              instance;
    uint32_t              pend_state           = 0;

    /* Find a secure peripheral which is handled only by secure target */
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_peripheral_hdr,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    minor_id = 1;
    for (per_num = 0; per_num < soc_peripheral_hdr->num;) {
        instance = 0;
        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, minor_id, instance),
                                            (uint8_t **)&soc_peripheral_desc,
                                            (uint32_t *)sizeof(soc_peripheral_desc_t));
            if (status == TBSA_STATUS_NOT_FOUND) {
                break;
            }

            if (soc_peripheral_desc->attribute == SECURE_PROGRAMMABLE) {
                if (soc_peripheral_desc->intr_target & SECURE_TARGET_ONLY) {
                    source_to_check = TRUE;

                    /* Set PEND bit in trusted mode */
                    status = val->interrupt_set_pending(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
                    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
                        return;
                    }

                    /* Read(in secure mode) the pending bit and confirm we have trusted interrupt in pending state */
                    status = val->interrupt_get_pending_status(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id), &pend_state);
                    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
                        return;
                    }

                    if(pend_state) {
                        /* Read(in non-secure mode) the pending bit and confirm we read zero */
                        status = val_interrupt_get_pending_status(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id), &pend_state);
                        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                            return;
                        }

                        if(!pend_state) {
                            /* Route the trusted interrupt to non-secure target(done in secure mode) */
                            status = val->interrupt_route(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id), PE_NONSECURE);
                            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                                return;
                            }

                            /* Read(in non-secure mode) the pending bit and confirm we read pending bit not set */
                            status = val_interrupt_get_pending_status(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id), &pend_state);
                            if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                                return;
                            }

                            if(pend_state) {
                                val->err_check_set(TEST_CHECKPOINT_7, TBSA_STATUS_INCORRECT_VALUE);
                                status = val->interrupt_clear_pending(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
                                if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                                    return;
                                }
                                return;
                            }
                        }
                    } else {
                        val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_INCORRECT_VALUE);
                        status = val->interrupt_clear_pending(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
                        if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
                            return;
                        }
                        return;
                    }
                    status = val->interrupt_clear_pending(EXCP_NUM_EXT_INT(soc_peripheral_desc->intr_id));
                    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                        return;
                    }
                }
            }

            instance++;
        } while (instance < GET_NUM_INSTANCE(soc_peripheral_desc));

        minor_id++;
        if (status == TBSA_STATUS_NOT_FOUND) {
            continue;
        } else {
            per_num += GET_NUM_INSTANCE(soc_peripheral_desc);
        }
    }

    if(source_to_check) {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        val->print(PRINT_ALWAYS, "\n\r\tNo available resource to check", 0);
        val->set_status(RESULT_SKIP(status));
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
