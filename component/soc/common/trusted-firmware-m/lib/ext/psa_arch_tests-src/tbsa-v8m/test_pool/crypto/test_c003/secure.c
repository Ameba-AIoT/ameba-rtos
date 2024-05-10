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

#define KEY_SIZE  32

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 3),
                  CREATE_TEST_TITLE("HUK should be in Confidential-Lockable-Bulk fuses, accessible only to TW"),
                  CREATE_REF_TAG("R220/R240_TBSA_KEY"),
                  entry_hook,
                  test_payload,
                  exit_hook);


tbsa_val_api_t *g_val;
memory_desc_t  *nvram_desc;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void hard_fault_esr (unsigned long *sf_args)
{
    g_val->print(PRINT_DEBUG, "\n\r\tHardFault triggered when HUK was accessed from"
                 "non-Trusted world", 0);

    __asm volatile("DSB");
    __asm volatile("ISB");

    g_val->system_reset(WARM_RESET);

    while(1);
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

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    key_desc_t    *key_info_huk;
    uint32_t      expected_fuse_type, instance = 0;
    uint32_t      key[KEY_SIZE];
    uint32_t      active_test;
    uint32_t      key_mask;
    uint32_t      result;
    uint32_t      i;

    g_val = val;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&nvram_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    if (active_test == TBSA_NS_TEST_ACTIVE) {
        /* Non-secure is active - so skipping Secure test */
        return;
    }

    status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    status = val->crypto_get_key_info(&key_info_huk, HUK, &instance);
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    expected_fuse_type = FUSE_CONFIDENTIAL | FUSE_BULK | FUSE_LOCKABLE;

    if ((key_info_huk->type & expected_fuse_type) != expected_fuse_type) {
        val->print(PRINT_ERROR, "\n\r\tFuse type in which HUK stored is non-compliant", 0);
        val->print(PRINT_ERROR, "\n\r\tFuse type %x", key_info_huk->type);
        val->print(PRINT_ERROR, "\n\r\tExpected Fuse type %x", expected_fuse_type);
        val->err_check_set(TEST_CHECKPOINT_5, TBSA_STATUS_ERROR);
        return;
    }

    if ((key_info_huk->state & FUSE_OPEN) == FUSE_OPEN) {
        status = val->fuse_ops(FUSE_READ, key_info_huk->addr, key, MIN(KEY_SIZE, key_info_huk->size));
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        result   = 0;
        key_mask = 0;
        /* check whether key holds the default value, if so warn ! */
        for (i = 0; i < MIN(KEY_SIZE, key_info_huk->size); i++) {
            if (key[i] == key_info_huk->def_val) {
               result |= (1U << i);
            }
            key_mask |= (1U << i);
        }

        if (result == key_mask) {
            val->print(PRINT_ALWAYS, "\n\r\tHUK having the default value : %X", key_info_huk->def_val);
        }

    } else {
        val->print(PRINT_ALWAYS, "\n\r\tHUK is not open", 0);
        val->set_status(RESULT_SKIP(1));
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      active_test;

    active_test = TBSA_NS_TEST_ACTIVE;
    status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }
}
