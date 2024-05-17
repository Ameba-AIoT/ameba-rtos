/** @file
 * Copyright (c) 2018-2020, Arm Limited or its affiliates. All rights reserved.
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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 5),
                  CREATE_TEST_TITLE("Check transaction filters config space must only be accessed from the TW"),
                  CREATE_REF_TAG("R060/R080/R210_TBSA_INFRA-R110_TBSA_FUSE"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;

void hard_fault_esr (unsigned long *sf_args)
{
    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

    /* Updating the return address in the stack frame in order to avoid periodic fault */
    sf_args[6] = sf_args[6] + 4;
}

__attribute__((naked)) void HF_Handler(void)
{
    __asm volatile("mrs r0, control_ns \n"
                   "mov r1, #0x2       \n"
                   "and r0, r1         \n"
                   "cmp r0, r1         \n"
                   "beq _psp_ns        \n"
                   "mrs r0, msp_ns     \n"
                   "b hard_fault_esr \n"
                   "_psp_ns:           \n"
                   "mrs r0, psp_ns     \n"
                   "b hard_fault_esr \n");
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
