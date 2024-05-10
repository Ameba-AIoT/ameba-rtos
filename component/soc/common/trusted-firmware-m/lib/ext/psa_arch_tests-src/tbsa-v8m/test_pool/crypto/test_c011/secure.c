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

#define FUSE_SIZE  32

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_CRYPTO_BASE, 11),
                  CREATE_TEST_TITLE("Check a confidential SW receipient fuse is readable by privileged software."),
                  CREATE_REF_TAG("R100_TBSA_FUSE"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t         *memory_desc;
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
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);
    g_val = val;

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t   status;
    fuse_desc_t     *fuse_desc;
    uint32_t        data[FUSE_SIZE], data1[FUSE_SIZE];
    boot_t          boot;
    uint32_t        shcsr = 0UL;
    uint32_t        control, i;

    val->memset(data1, 0x0, sizeof(data1)/sizeof(uint32_t));

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    if (boot.wb != WARM_BOOT_REQUESTED) {

        status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
        if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
            return;
        }

        status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }

        boot.wb = WARM_BOOT_REQUESTED;
        /* Saving the boot state */
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }

        /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
        status = val->mem_reg_read(SHCSR, &shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            return;
        }

        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            return;
        }

        status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            return;
        }

        /* Obtain the confidential fuse details from target config */
        status = val->get_fuse_info((fuse_desc_t **)&fuse_desc, FUSE_CONFIDENTIAL, 0);
        if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
            return;
        }

        status = val->fuse_ops(FUSE_READ, fuse_desc->addr, data, MIN(FUSE_SIZE, fuse_desc->size));
        if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
            return;
        }

        for (i = 0; i < MIN(FUSE_SIZE, fuse_desc->size); i++) {
            if (data[i] == fuse_desc->def_val) {
                val->print(PRINT_ERROR, "\n\r\tThe given fuse is empty", 0);
                val->err_check_set(TEST_CHECKPOINT_B, TBSA_STATUS_ERROR);
                return;
            }
        }

        /* Change the mode to unprivilege access */
        __asm volatile ("MRS %0, control" : "=r" (control));
        control |= 0x1;
        __asm volatile ("MSR control, %0" : : "r" (control) : "memory");

        /* Performing unprivilege access to Confidential fuse */
        status = val->fuse_ops(FUSE_READ, fuse_desc->addr, data1, MIN(FUSE_SIZE, fuse_desc->size));
        if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
            return;
        }

        for (i = 0; i < MIN(FUSE_SIZE, fuse_desc->size); i++) {
            if (data1[i] == data[i]) {
                val->print(PRINT_ERROR, "\n\r\tAble to access confidential fuse in unprivilege mode", 0);
                val->err_check_set(TEST_CHECKPOINT_D, TBSA_STATUS_ERROR);
                return;
            }
        }
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t   status;
    boot_t          boot;
    uint32_t        shcsr = 0UL;

    /* Restoring the state */
    boot.wb = BOOT_UNKNOWN;
    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
    if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_11, status)) {
        return;
    }
}
