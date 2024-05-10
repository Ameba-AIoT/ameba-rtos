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

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_DEBUG_BASE, 8),
                  CREATE_TEST_TITLE("Access to the secure DPM registers must be restricted to privileged Secure world software."),
                  CREATE_REF_TAG("R020/R280_TBSA_DEBUG"),
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
    boot_t          boot;
    uint32_t        shcsr = 0UL;
    uint32_t        control;
    uint32_t        dpm_instance = 0, dpm_total_instance;
    uint32_t        certificate_valid[10]={-1};
    dpm_hdr_t       *dpm_hdr;
    dpm_desc_t      *dpm_desc;


    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, 0),
                                    (uint8_t **)&dpm_hdr, (uint32_t *)sizeof(dpm_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    status = val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
    if(val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    if (boot.wb != WARM_BOOT_REQUESTED) {

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

          dpm_total_instance = GET_NUM_INSTANCE(dpm_hdr);

          for (dpm_instance = 0; dpm_instance < dpm_total_instance; dpm_instance++) {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, dpm_instance),
                                            (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
                return;
            }

            /* Check if the certificate given as per target config is valid using Public key
             * when accessed from Privileged mode */
            if (dpm_desc->unlock_token == TOKEN_CERTIFICATE) {

                 certificate_valid[dpm_instance] = val->crypto_validate_certificate(dpm_desc->certificate_addr, dpm_desc->public_key_addr, dpm_desc->certificate_size,dpm_desc->public_key_size);
                 if (val->err_check_set(TEST_CHECKPOINT_A, certificate_valid[dpm_instance])) {
                     return;
                 }

                 /* Change the mode to unprivilege access */
                 __asm volatile ("MRS %0, control" : "=r" (control));
                 control |= 0x1;
                 __asm volatile ("MSR control, %0" : : "r" (control) : "memory");

                 /* Performing unprivilege access to DPM */
                 certificate_valid[dpm_instance] = val->crypto_validate_certificate(dpm_desc->certificate_addr, dpm_desc->public_key_addr, dpm_desc->certificate_size,dpm_desc->public_key_size);
                 if (certificate_valid[dpm_instance] == TRUE) {
                     val->err_check_set(TEST_CHECKPOINT_B, 1);
                     return;
                 }
            }
          }
    }

    /* Restoring the state */
    boot.wb = BOOT_UNKNOWN;
    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
    if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t   status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return;
    }
}
