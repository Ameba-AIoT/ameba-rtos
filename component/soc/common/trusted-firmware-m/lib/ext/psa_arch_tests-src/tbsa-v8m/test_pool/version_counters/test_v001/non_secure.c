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

#define TF_MIN_VER_CNT    64
#define NTF_MIN_VER_CNT   256

/**
  Publish these functions to the external world as associated to this test ID
**/
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_VERSION_COUNTERS_BASE, 1),
                  CREATE_TEST_TITLE("Check version counter functionality"),
                  CREATE_REF_TAG("R010/R020/R030/R040/R050/R060_TBSA_COUNT"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t         *memory_desc;
miscellaneous_hdr_t   *misc;
miscellaneous_desc_t  *misc_desc;

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
    uint32_t      instance = 0;
    uint32_t      fw_ver_count, fused_ver_count = 0;
    boot_t        boot;
    uint32_t      shcsr = 0;

    status = val->crypto_set_base_addr(SECURE_PROGRAMMABLE);
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                    (uint8_t **)&memory_desc,
                                    (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    if (boot.cb != COLD_BOOT_REQUESTED) {
        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_VER_COUNT, instance),
                                            (uint8_t **)&misc_desc,
                                            (uint32_t *)sizeof(miscellaneous_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
                return;
            }

            /* Checking for on-chip non-volatile version counter range */
            if ((misc_desc->fw_ver_type == TRUSTED) || (misc_desc->fw_ver_type == NON_TRUSTED)) {
                if (misc_desc->fw_ver_type == TRUSTED) {
                    if (misc_desc->fw_ver_cnt_max < TF_MIN_VER_CNT) {
                        val->print(PRINT_DEBUG, "\n\r\tTrusted firmware version counter max should be >= %d", TF_MIN_VER_CNT);
                        val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_INCORRECT_VALUE);
                        return;
                    }
                } else {
                    if (misc_desc->fw_ver_cnt_max < NTF_MIN_VER_CNT) {
                        val->print(PRINT_DEBUG, "\n\r\tNon-trusted firmware version counter max should be >= %d", NTF_MIN_VER_CNT);
                        val->err_check_set(TEST_CHECKPOINT_7, TBSA_STATUS_INCORRECT_VALUE);
                        return;
                    }
                }
            }

            /* Reading the current firmware version counter */
            fw_ver_count = val->firmware_version_read(instance, misc_desc->fw_ver_type);

            /* Skip the test if max version was already fused */
            if (fw_ver_count == misc_desc->fw_ver_cnt_max) {
                val->set_status(RESULT_SKIP(status));
                return;
            }

            /* Updating the firmware version count(trusted mode) */
            val->firmware_version_update(instance, misc_desc->fw_ver_type, ++fw_ver_count);

            /* Check if the versoin is incremented */
            fused_ver_count = val->firmware_version_read(instance, misc_desc->fw_ver_type);
            if (fused_ver_count != (fw_ver_count)) {
                val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_INVALID);
                return;
            }

            /* Updating firmware version(lower than previous counter)
             * which confirms we cannot decrement firmware version counter
             */
            val->firmware_version_update(instance, misc_desc->fw_ver_type, --fw_ver_count);
            fused_ver_count = val->firmware_version_read(instance, misc_desc->fw_ver_type);
            if (fused_ver_count != (++fw_ver_count)) {
	            val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_INVALID);
                return;
            }

            /* Updating the firmware version count to max */
            val->firmware_version_update(instance, misc_desc->fw_ver_type, misc_desc->fw_ver_cnt_max);
            fused_ver_count = val->firmware_version_read(instance, misc_desc->fw_ver_type);
            if (fused_ver_count != misc_desc->fw_ver_cnt_max) {
                val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_INVALID);
                return;
            }

            /* When a version counter reaches its maximum value, it must not roll over,
             * and no further changes must be possible
             */
            status = val->firmware_version_update(instance, misc_desc->fw_ver_type, (misc_desc->fw_ver_cnt_max + 1));
            fused_ver_count = val->firmware_version_read(instance, misc_desc->fw_ver_type);
            if (misc_desc->fw_ver_cnt_max != fused_ver_count) {
                val->err_check_set(TEST_CHECKPOINT_B, TBSA_STATUS_INVALID);
                return;
            }

            /* Updating the firmware version count(non-trusted mode) */
            if (instance == (GET_NUM_INSTANCE(misc_desc) - 1)) {
                boot.cb = COLD_BOOT_REQUESTED;
                status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
                if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
                    return;
                }

                /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
                status = val->mem_reg_read(SHCSR, &shcsr);
                if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
                    return;
                }

                status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
                if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
                    return;
                }

                status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
                if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
                    return;
                }

                val_crypto_set_base_addr(SECURE_PROGRAMMABLE);

                /* Updating the firmware version count(non-trusted mode), secure-fault is expected! */
                val_firmware_version_update(instance, misc_desc->fw_ver_type, ++fw_ver_count);

                /* Test shouldn't come here */
                val->print(PRINT_ERROR, "\n\r\t Error: able to update the version counter from NTW", 0);
                val->err_check_set(TEST_CHECKPOINT_10, TBSA_STATUS_ERROR);
                return;
            }

            /* confirms roll over didn't happen */
            instance++;
        } while (instance < GET_NUM_INSTANCE(misc_desc));
        /* repeat test for remaining available version counters */
    } else {
        boot.cb = BOOT_UNKNOWN;
        status = val->nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_11, status)) {
            return;
        }

        status = val->nvram_read(memory_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
        if (val->err_check_set(TEST_CHECKPOINT_12, status)) {
            return;
        }

        /* Restoring faults */
        status = val->mem_reg_write(SHCSR, shcsr);
        if (val->err_check_set(TEST_CHECKPOINT_13, status)) {
            return;
        }

        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MISCELLANEOUS, MISCELLANEOUS_VER_COUNT, instance),
                                            (uint8_t **)&misc_desc,
                                            (uint32_t *)sizeof(miscellaneous_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_14, status)) {
                return;
            }

            /* Check if version is preserved across reset */
            if(misc_desc->fw_ver_cnt_max != val->firmware_version_read(instance, misc_desc->fw_ver_type)) {
                val->err_check_set(TEST_CHECKPOINT_15, TBSA_STATUS_INVALID);
                return;
            }
            instance++;
        } while (instance < GET_NUM_INSTANCE(misc_desc));
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_16, status)) {
        return;
    }
}
