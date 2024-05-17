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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 6),
                  CREATE_TEST_TITLE("A Trusted HK must not be directly accessible by any software."),
                  CREATE_REF_TAG("R140_TBSA_KEY-R100_TBSA_BOOT"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t *g_val;
memory_desc_t  *nvram_desc;

void hard_fault_esr (unsigned long *sf_args)
{
    /* check agianst case 3 */
    g_val->print(PRINT_ALWAYS, "\n\r\tHardFault triggered when HUK was accessed from software", 0);

    /* Issue system warm reset */
    g_val->system_reset(WARM_RESET);

    /* Shouldn't come here */
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

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    g_val = val;

    val->test_initialize(&init);

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));

}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    key_desc_t    *key_info_trust;
    uint32_t      i, instance = 0;
    uint32_t      key[KEY_SIZE];
    uint32_t      shcsr;
    uint32_t      trusted_hw_key_found;
    uint32_t      active_test;
    boot_t        boot;
    bool_t        check_done = FALSE;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&nvram_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    do {
        status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
            return;
        }

        status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        if (active_test == TBSA_NS_TEST_ACTIVE) {
            /* Non-secure is active - so skipping Secure test */
            return;
        }

        if (boot.wb != WARM_BOOT_REQUESTED) {
            instance = 0x0;
            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
            if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }

            trusted_hw_key_found = 0x0;
            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD2), &trusted_hw_key_found, sizeof(trusted_hw_key_found));
            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                return;
            }

            status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
            if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                return;
            }

            active_test = TBSA_S_TEST_ACTIVE;
            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
            if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                return;
            }
        }

        status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            return;
        }

        status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
        if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
            return;
        }

        status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD2), &trusted_hw_key_found, sizeof(trusted_hw_key_found));
        if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
            return;
        }

        status = val->crypto_get_key_info((key_desc_t **)&key_info_trust, TRUST, &instance);
        if ((status != TBSA_STATUS_SUCCESS) && (!trusted_hw_key_found)) {
            val->print(PRINT_ALWAYS, "\n\r\tTrusted HW Key is not available to check", 0);
            val->set_status(RESULT_SKIP(status));
            return;
        }
        if (status != TBSA_STATUS_SUCCESS) {
            return;
        }

        /* Writing next instance value into nvram */
        ++instance;
        status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
        if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
            return;
        }

        status = val->mem_reg_read(SHCSR, &shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
            return;
        }

        if (boot.wb != WARM_BOOT_REQUESTED) {
            /* Write WARM_BOOT_REQUESTED only for the first instance found */
            boot.wb = WARM_BOOT_REQUESTED;
            /* Saving the boot state */
            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
            if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
                return;
            }

            trusted_hw_key_found = 0x1;
            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD2), &trusted_hw_key_found, sizeof(trusted_hw_key_found));
            if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
                return;
            }


            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
            if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
                return;
            }
        }

        /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
        status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
        if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
            return;
        }


        /* Initializing the key buffer with known value - zero */
        val->memset((void*)key, 0x0, MIN(KEY_SIZE, key_info_trust->size));

        /* Three things can happen:
         * 1. Returns error status
         * 2. Reads the key
         * 3. Fault may occur
         */
        status = val->fuse_ops(FUSE_READ, key_info_trust->addr, key, MIN(KEY_SIZE, key_info_trust->size));
        if (status != TBSA_STATUS_SUCCESS) {
            for (i=0; i < MIN(KEY_SIZE, key_info_trust->size); i++) {
                if(key[i] != 0) {
                    /* Checks against case 1 */
                    val->print(PRINT_ERROR, "\n\r\tTrusted key is accessible", 0);
                    val->err_check_set(TEST_CHECKPOINT_11, TBSA_STATUS_ERROR);
                    return;
                }
            }
            check_done = TRUE;
            val->print(PRINT_DEBUG, "\n\r\tfuse_read returned error status without updating the key buffer", 0);
        }

        if(!check_done) {
            for (i = 0; i < MIN(KEY_SIZE, key_info_trust->size); i++) {
                if (key[i] != 0x0) {
                    /* Checks against case 2 */
                    val->print(PRINT_ERROR, "\n\r\tTrusted key is accessible", 0);
                    val->err_check_set(TEST_CHECKPOINT_12, TBSA_STATUS_ERROR);
                    return;
                }
            }
            val->print(PRINT_DEBUG, "\n\r\tfuse_read returned success status and key buffer is zero", 0);
        }
    }while(instance < GET_NUM_INSTANCE(key_info_trust));

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      shcsr = 0UL;
    uint32_t      active_test;
    boot_t        boot;

    status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
    if (val->err_check_set(TEST_CHECKPOINT_13, status)) {
        return;
    }

    if (active_test == TBSA_S_TEST_ACTIVE) {

        active_test = TBSA_NS_TEST_ACTIVE;
        status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_ACT_TST), &active_test, sizeof(active_test));
        if (val->err_check_set(TEST_CHECKPOINT_14, status)) {
            return;
        }

        /* Restoring the state */
        boot.wb = BOOT_UNKNOWN;
        status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_15, status)) {
            return;
        }

        status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_16, status)) {
            return;
        }

        /* Restoring faults */
        status = val->mem_reg_write(SHCSR, shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_17, status)) {
            return;
        }
    }
}
