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

tbsa_val_api_t        *g_val;

void HF_Handler(void)
{
    /* Issue system warm reset */
    g_val->system_reset(WARM_RESET);

    /* Shouldn't come here */
    while(1);
}

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    g_val  = val;
    status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
    if(val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
