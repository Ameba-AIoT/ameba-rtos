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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_DEBUG_BASE, 5),
                  CREATE_TEST_TITLE("DPM default state check"),
                  CREATE_REF_TAG("R050/R090/R150_TBSA_DEBUG"),
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
    uint32_t      dpm_instance, dpm_enable, dpm_status;
    dpm_hdr_t     *dpm_hdr;
    dpm_desc_t    *dpm_desc;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, 0, 0),
                                    (uint8_t **)&dpm_hdr, (uint32_t *)sizeof(dpm_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    /* Check if DPM is present.*/
    if (!dpm_hdr->num) {
        val->print(PRINT_ERROR, "\nNo DPM present in the platform", 0);
        val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_NOT_FOUND);
        return;
    }

    /* Check the default behavior of the DPMs */
    for (dpm_instance = 0; dpm_instance < dpm_hdr->num; dpm_instance++) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, dpm_instance),
                                        (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }
        /* Get the current state of the DPM under check*/
        status = val->dpm_get_state(dpm_desc->index, &dpm_status);
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            return;
        }

        if ((dpm_status & DPM_EN_IMPLEMENTED) != DPM_EN_IMPLEMENTED) {
            val->print(PRINT_ERROR, "\n        dpm_en not implemented for DPM %d", dpm_desc->index);
            val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
            return;
        }

        if (dpm_desc->is_controlling_secure) {
            if ((dpm_status & DPM_LOCK_IMPLEMENTED) != DPM_LOCK_IMPLEMENTED) {
                val->print(PRINT_ERROR, "\n        dpm_lock not implemented for DPM %d", dpm_desc->index);
                val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
                return;
            }
            dpm_enable = DPM_EN_IMPLEMENTED|DPM_EN_STATE;
            if (((dpm_status & DPM_LOCK_STATE) != DPM_LOCK_STATE) || ((dpm_status & dpm_enable) != dpm_enable)) {
                val->print(PRINT_ERROR, "\n        DPM default state is not closed or locked for DPM %d", dpm_desc->index);
                val->set_status(RESULT_FAIL(TBSA_STATUS_ERROR));
                return;
            }
        }
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    return;
}

void exit_hook(tbsa_val_api_t *val)
{
}
