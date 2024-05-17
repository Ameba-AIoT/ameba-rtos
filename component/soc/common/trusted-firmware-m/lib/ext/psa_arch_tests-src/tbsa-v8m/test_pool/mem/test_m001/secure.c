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

#define PATTERN 0x12345678u

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
bool_t         secure_range_found = FALSE;

void hard_fault_esr (unsigned long *sf_args)
{

    if (IS_TEST_PENDING(g_val->get_status())) {
        g_val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    }

    /* Updating the return address in the stack frame in order to avoid periodic fault */
    sf_args[6] = sf_args[6] + 4;
}

__attribute__((naked))
void HF_Handler(void)
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

tbsa_status_t setup_ns_env(void)
{
    tbsa_status_t status;

    /* Installing Trusted Fault Handler for NS test */
    status = g_val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
    if(g_val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return status;
    }

    return TBSA_STATUS_SUCCESS;
}



void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__,
                            };

    val->test_initialize(&init);

    g_val = val;
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    uint32_t       instance = 0;
    memory_desc_t  *mem_desc;
    tbsa_status_t  status;

    secure_range_found = FALSE;

    /* Read all the memory ranges given to us and ensure at least 1 range is secure */
    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_SRAM, instance),
                                        (uint8_t **)&mem_desc, (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
            return;
        }

        if (mem_desc->attribute == MEM_SECURE) {
            if (val->is_secure_address(mem_desc->start)) {
                secure_range_found = TRUE;
                break;
            } else {
                val->err_check_set(TEST_CHECKPOINT_2, TBSA_STATUS_ERROR);
                return;
            }
        }
        instance++;
    }while (instance < GET_NUM_INSTANCE(mem_desc));

    if (secure_range_found != TRUE) {
        val->err_check_set(TEST_CHECKPOINT_3, TBSA_STATUS_NOT_FOUND);
        return;
    }

    if(setup_ns_env()) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}
