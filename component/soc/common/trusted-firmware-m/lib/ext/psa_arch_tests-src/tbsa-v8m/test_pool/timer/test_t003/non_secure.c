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

#define TIMEOUT (0xFFFFFFFFUL)

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_TRUSTED_TIMERS_BASE, 3),
                  CREATE_TEST_TITLE("Trusted and Non-trusted world operation to TRTC"),
                  CREATE_REF_TAG("R130/R150/R160_TBSA_TIME"),
                  entry_hook,
                  test_payload,
                  exit_hook);

soc_peripheral_hdr_t  *soc_per;
soc_peripheral_desc_t *soc_per_desc;
memory_desc_t         *memory_desc;
uint32_t              shcsr;

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
    uint32_t      instance;
    uint32_t      data;
    bool_t        trtc_found = FALSE;
    boot_t        boot;
    uint32_t      timeout    = TIMEOUT;

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

    if (boot.cb != COLD_BOOT_REQUESTED) {
        instance = 0UL;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            return;
        }
    } else {
        /* Getting instance value from NVRAM */
        status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }
    }

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_RTC, instance),
                                        (uint8_t **)&soc_per_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            break;
        }

        if (soc_per_desc->attribute == SECURE_PROGRAMMABLE) {
            trtc_found = TRUE;
            if (boot.cb != COLD_BOOT_REQUESTED) {
                boot.cb = COLD_BOOT_REQUESTED;
                status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
                if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                    return;
                }
                /* Writing instance value into NVRAM so that we go to next instance of TRTC in next run after reset */
                status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &instance, sizeof(instance));
                if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                    return;
                }
                /* Issuing cold boot request */
                val->system_reset(COLD_RESET);
                /* Shouldn't come here */
                val->print(PRINT_ERROR, "\n\r\tShouldn't comer here", 0);
                val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_ERROR);
                return;
            }

            /* Check for validity of TRTC ? */
            if (val->is_rtc_synced_to_server(soc_per_desc->base)) {
                if (!val->is_rtc_trustable(soc_per_desc->base)) {
                    val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_INCORRECT_VALUE);
                    boot.cb = BOOT_UNKNOWN;
                    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot));
                    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                        return;
                    }
                    break;
                }
            } else {
                if (val->is_rtc_trustable(soc_per_desc->base)) {
                    /* When there is outage of power to the TRTC, TRTC is not trusted */
                    val->err_check_set(TEST_CHECKPOINT_C, TBSA_STATUS_INCORRECT_VALUE);
                    boot.cb = BOOT_UNKNOWN;
                    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot));
                    if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
                        return;
                    }
                    break;
                }
                val->print(PRINT_DEBUG, "\n\r\tRTC is not trustable - not synced with server!", 0);
            }
            /* Indicates TRTC is not Trusted when there is outage of power to TRTC */
            boot.cb = COLD_BOOT_COMPLETED;
        }
        instance++;
    } while(instance < GET_NUM_INSTANCE(soc_per_desc));

    if (!trtc_found) {
        return;
    }

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return;
    }

    instance      = 0UL;
    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, SOC_PERIPHERAL_RTC, instance),
                                        (uint8_t **)&soc_per_desc,
                                        (uint32_t *)sizeof(soc_peripheral_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
            break;
        }

        if (soc_per_desc->attribute == SECURE_PROGRAMMABLE) {
            /* Ensure TRTC access from non-trusted world triggers secure fault exception */
            val->set_status(RESULT_PENDING(status));

            /* Trying to read the TRTC base address, expect secure fault? */
            val_mem_read_wide((uint32_t *)(soc_per_desc->base + soc_per_desc->offset), &data);

            /* wait here till pending status is cleared by secure fault */
            while (IS_TEST_PENDING(val->get_status()) && --timeout);

            if(!timeout) {
                val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing TRTC address 0x%X from NT world!", (uint32_t)(soc_per_desc->base));
                val->err_check_set(TEST_CHECKPOINT_11, TBSA_STATUS_TIMEOUT);
                return;
            }

            val->set_status(RESULT_PENDING(status));

            timeout = TIMEOUT;
            /* Trying to read the clock source base address for a given TRTC, expect secure fault? */
            val_mem_read_wide((uint32_t *)soc_per_desc->clk_src, &data);

            /* wait here till pending status is cleared by secure fault */
            while (IS_TEST_PENDING(val->get_status()) && --timeout);

            if(!timeout) {
                val->print(PRINT_ERROR, "\n\r\tNo fault occurred when accessing clock source for TRTC 0x%X from NT world!", (uint32_t)(soc_per_desc->base));
                val->err_check_set(TEST_CHECKPOINT_12, TBSA_STATUS_TIMEOUT);
                return;
            }
        }
        instance++;
        timeout = TIMEOUT;
    } while(instance < GET_NUM_INSTANCE(soc_per_desc));

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    boot_t        boot;
    uint32_t      default_val;

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_13, status)) {
        return;
    }

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_14, status)) {
        return;
    }

    boot.cb = BOOT_UNKNOWN;
    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot));
    if (val->err_check_set(TEST_CHECKPOINT_15, status)) {
        return;
    }

    default_val = 0xFFFFFFFFUL;
    status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SPAD1), &default_val, sizeof(default_val));
    if (val->err_check_set(TEST_CHECKPOINT_16, status)) {
        return;
    }
}
