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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_INTERRUPTS_BASE, 3),
                  CREATE_TEST_TITLE("Check status flag of trusted interrupt event from NTW configured by TW"),
                  CREATE_REF_TAG("R150_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_desc_t *timer_desc;
bool_t                trusted_timer_found = FALSE;

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
    uint32_t      instance       = 0;
    uint32_t      pend_state     = 0;

    /* Check we have at least one trusted timer */
    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_TIMER, instance),
                                        (uint8_t **)&timer_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
            return;
        }

        if (timer_desc->attribute == SECURE_PROGRAMMABLE) {
            /* So we have one trusted timer, let's work with that */
            trusted_timer_found = TRUE;
            break;
        }

        instance++;
    } while(instance < GET_NUM_INSTANCE(timer_desc));

    if (trusted_timer_found) {
        status = val->interrupt_set_pending(EXCP_NUM_EXT_INT(timer_desc->intr_id));
        if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
            return;
        }

        /* Read(in secure mode) the pending bit and confirm we have trusted interrupt in pending state */
        status = val->interrupt_get_pending_status(EXCP_NUM_EXT_INT(timer_desc->intr_id), &pend_state);
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        if (pend_state) {
            /* Read(in non-secure mode) the pending bit and confirm we read zero */
            status = val_interrupt_get_pending_status(EXCP_NUM_EXT_INT(timer_desc->intr_id), &pend_state);
            if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }

            if (!pend_state) {
                /* Route the trusted interrupt to non-secure target(done in secure mode) */
                status = val->interrupt_route(EXCP_NUM_EXT_INT(timer_desc->intr_id), PE_NONSECURE);
                if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                    return;
                }

                /* Read(in non-secure mode) the pending bit and confirm we read pending bit set */
                status = val_interrupt_get_pending_status(EXCP_NUM_EXT_INT(timer_desc->intr_id), &pend_state);
                if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                    return;
                }

                if(pend_state) {
                    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
                } else {
                    val->err_check_set(TEST_CHECKPOINT_7, TBSA_STATUS_INCORRECT_VALUE);
                }
            } else {
                val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_INCORRECT_VALUE);
            }
        } else {
            val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_INCORRECT_VALUE);
        }
    } else {
        /* We did not find trusted timer to test */
        val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_NOT_FOUND);
    }
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* clean up */
    status = val->interrupt_route(EXCP_NUM_EXT_INT(timer_desc->intr_id), PE_SECURE);
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }

    status = val->interrupt_clear_pending(EXCP_NUM_EXT_INT(timer_desc->intr_id));
    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return;
    }
}
