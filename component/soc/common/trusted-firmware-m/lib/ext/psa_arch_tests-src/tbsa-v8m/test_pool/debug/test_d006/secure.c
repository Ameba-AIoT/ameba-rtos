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

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_DEBUG_BASE, 6),
                  CREATE_TEST_TITLE("Password unlock token requirements"),
                  CREATE_REF_TAG("R200/R210_TBSA_DEBUG"),
                  entry_hook,
                  test_payload,
                  exit_hook);

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
    uint32_t      dpm_instance, target_dpm, dpm_status, dpm_enable, dpm_lock;
    dpm_hdr_t     *dpm_hdr;
    dpm_desc_t    *dpm_desc, *target_dpm_desc;
    bool_t        unlock_token_password = FALSE;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, 0, 0),
                                    (uint8_t **)&dpm_hdr, (uint32_t *)sizeof(dpm_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    /* Check if DPM is present.*/
    if (!dpm_hdr->num) {
        val->print(PRINT_ERROR, "\n\t\rNo DPM present in the platform", 0);
        val->err_check_set(TEST_CHECKPOINT_2, TBSA_STATUS_NOT_FOUND);
        return;
    }

    /* Check the default behavior of the DPMs */
    for (dpm_instance = 0; dpm_instance < dpm_hdr->num; dpm_instance++) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, dpm_instance),
                                        (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        if (dpm_desc->unlock_token == TOKEN_PASSWD) {
            unlock_token_password = TRUE;

            /* Compare if the password size is greater than 16 bytes (128-bits) */
            if (dpm_desc->passwd_size < 16) {
                val->print(PRINT_ERROR, "\n\r\tPassword size for unlock token is less than 128 bits for DPM %d", dpm_desc->index);
                val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
                return;
            }
            if (dpm_hdr->num == 1) {
                val->print(PRINT_ALWAYS, "\n\r\tUnique password unlock token since only one DPM is present", 0);
                val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
                return;
            }
            /* Use this password to unlock other DPMs, if any DPM gets unlocked then fail the test.*/
            for (target_dpm = 0; target_dpm < dpm_hdr->num; target_dpm++) {
                if (target_dpm == dpm_instance)
                    continue;

                status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, target_dpm),
                                                (uint8_t **)&target_dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
                if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                    return;
                }
                if (dpm_desc->unlock_token == TOKEN_PASSWD) {
                    status = val->dpm_get_state(target_dpm_desc->index, &dpm_status);
                    if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                        return;
                    }
                    dpm_enable = DPM_EN_IMPLEMENTED|DPM_EN_STATE;
                    dpm_lock = DPM_LOCK_IMPLEMENTED|DPM_LOCK_STATE;

                    /* If the DPM state is locked then continue with other DPMs*/
                    if ((dpm_status & dpm_lock) == dpm_lock)
                        continue;
                    else if ((dpm_status & dpm_enable) != dpm_enable) {
                        /* Set the DPM state to Closed, to check if the unlocking can be done by using
                        other DPM's password.*/
                        status = val->dpm_set_state(target_dpm_desc, DPM_CLOSED_STATE);
                        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                            return;
                        }
                    }

                    /* Try to unlock DPM using other DPM's password.*/
                    status = val->dpm_set_state(target_dpm_desc, DPM_OPEN_STATE);
                    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                        return;
                    }
                    status = val->dpm_get_state(target_dpm_desc->index, &dpm_status);
                    if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                        return;
                    }
                    if ((dpm_status & dpm_enable) != dpm_enable) {
                        val->print(PRINT_ERROR, "\n\r\tPassword of DPM %d was able to unlock DPM", dpm_desc->index);
                        val->print(PRINT_ERROR, "%d", target_dpm_desc->index);
                        val->set_status(RESULT_FAIL(TBSA_STATUS_SUCCESS));
                        return;
                    }

                }
            }
        }
    }

    if (unlock_token_password) {
        val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    } else {
        val->set_status(RESULT_SKIP(TBSA_STATUS_NOT_FOUND));
        val->print(PRINT_ALWAYS, "\n\r\tDPM available are not TOKEN PASSWORD based", 0);
    }

    return;
}

void exit_hook(tbsa_val_api_t *val)
{
}
