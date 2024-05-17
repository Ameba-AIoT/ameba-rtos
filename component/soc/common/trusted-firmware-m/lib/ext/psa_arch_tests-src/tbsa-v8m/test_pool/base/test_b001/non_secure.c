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
/* Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 1),
                  CREATE_TEST_TITLE("Check Trusted and Non-trusted world asset access"),
                  CREATE_REF_TAG("R010/R020_TBSA_BASE-R010/R020/R030/R170/R220_TBSA_INFRA-R040_TBSA_EIP"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_hdr_t          *memory_hdr;
memory_desc_t         *memory_desc;
soc_peripheral_hdr_t  *soc_peripheral_hdr;
soc_peripheral_desc_t *soc_peripheral_desc;
uint32_t              shcsr;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t   status;
    uint32_t        data = 0;
    uint32_t        per_num = 0, region_num = 0, instance = 0, minor_id = 1;

    /* Get total number of memory regions from targetConfig*/
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, 0, 0),
                                      (uint8_t **)&memory_hdr,
                                      (uint32_t *)sizeof(memory_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
        return;
    }

    /* Perform read access on all address regions, and a write access only when the region is
       marked as normal read-write */
    for (region_num = 0; region_num < memory_hdr->num;) {
        instance = 0;
        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, minor_id, instance),
                                              (uint8_t **)&memory_desc,
                                              (uint32_t *)sizeof(memory_desc_t));
            if (status == TBSA_STATUS_NOT_FOUND) {
                break;
            } else if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
                return;
            }
            if (minor_id == MEMORY_FLASH) { /* Flash reads/writes should use spi_read/spi_write*/
                break;
            }
            /* This status flag is used to check whether exception is expected or not.*/
            val->set_status(RESULT_PENDING(0));
            if (memory_desc->attribute != MEM_NONSECURE) {
                val->set_status(RESULT_PENDING(1));
            }

            val_mem_read_wide((uint32_t *)memory_desc->start, &data);

            /* Check the status updated by handler in case a securefault occurs */
            if (IS_TEST_FAIL(val->get_status())) {
                val->print(PRINT_ERROR, "\n\r\tSecurefault occured on reads at address 0x%x", (uint32_t)(memory_desc->start));
                return;
            }
            if (memory_desc->mem_type == TYPE_NORMAL_READ_WRITE) {
                val_mem_write_wide((uint32_t *)memory_desc->start, data);

                /* Check the status updated by handler in case a securefault occurs */
                if (IS_TEST_FAIL(val->get_status())) {
                    val->print(PRINT_ERROR, "\n\r\tSecurefault occured on writes at address 0x%x", (uint32_t)(memory_desc->start));
                    return;
                }
            }

            instance++;
        } while (instance < GET_NUM_INSTANCE(memory_desc));
        minor_id++;
        if (status != TBSA_STATUS_NOT_FOUND) {
            region_num += GET_NUM_INSTANCE(memory_desc);
        }
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, 0, 0),
                                    (uint8_t **)&soc_peripheral_hdr,
                                    (uint32_t *)sizeof(soc_peripheral_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }

    minor_id = 1;
    for (per_num = 0; per_num < soc_peripheral_hdr->num;) {
        instance = 0;
        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_SOC_PERIPHERAL, minor_id, instance),
                                            (uint8_t **)&soc_peripheral_desc,
                                            (uint32_t *)sizeof(soc_peripheral_desc_t));
            if (status == TBSA_STATUS_NOT_FOUND) {
                break;
            } else if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                return;
            }

            val->set_status(RESULT_PENDING(0));
            if ((soc_peripheral_desc->attribute == SECURE_ACCESS) ||
                (soc_peripheral_desc->attribute == SECURE_PROGRAMMABLE)) {
                val->set_status(RESULT_PENDING(1));
            }

            val_mem_read_wide((uint32_t *)soc_peripheral_desc->base, &data);

            /* Check the status updated by handler in case a securefault occurs */
            if (IS_TEST_FAIL(val->get_status())) {
                val->print(PRINT_ERROR, "\n\r\tSecurefault occured at peripheral address 0x%x", (uint32_t)(soc_peripheral_desc->base));
                return;
            }
            instance++;
        } while (instance < GET_NUM_INSTANCE(soc_peripheral_desc));
        minor_id++;
        if (status != TBSA_STATUS_NOT_FOUND) {
            per_num += GET_NUM_INSTANCE(soc_peripheral_desc);
        }
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return;
    }

    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
        return;
    }
}
