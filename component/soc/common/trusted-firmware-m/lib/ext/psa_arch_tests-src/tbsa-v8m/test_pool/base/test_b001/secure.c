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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 1),
                  CREATE_TEST_TITLE("Check Trusted and Non-trusted world asset access"),
                  CREATE_REF_TAG("R010/R020_TBSA_BASE-R010/R020/R030/R170/R220_TBSA_INFRA-R040_TBSA_EIP"),
                  entry_hook,
                  test_payload,
                  exit_hook);

tbsa_val_api_t        *g_val;
memory_hdr_t          *memory_hdr;
memory_desc_t         *memory_desc;
soc_peripheral_hdr_t  *soc_peripheral_hdr;
soc_peripheral_desc_t *soc_peripheral_desc;

void entry_hook(tbsa_val_api_t *val)
{
    tbsa_test_init_t init = {
                             .bss_start      = &__tbsa_test_bss_start__,
                             .bss_end        = &__tbsa_test_bss_end__
                            };

    val->test_initialize(&init);
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void hard_fault_esr(unsigned long *sf_args)
{
    if (TBSA_STATUS(g_val->get_status()) == 0) {
        g_val->set_status(RESULT_FAIL(TBSA_STATUS_UNEXPECTED_EXCP));
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
    return g_val->interrupt_setup_handler(EXCP_NUM_HF, 0, HF_Handler);
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t   status;
    uint32_t        data = 0;
    uint32_t        per_num = 0, region_num = 0, instance = 0, minor_id = 1;
    g_val = val;

    /* Get total number of memory regions from targetConfig*/
    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, 0, 0),
                                    (uint8_t **)&memory_hdr,
                                    (uint32_t *)sizeof(memory_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
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
            } else if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
                return;
            }
            if (minor_id == MEMORY_FLASH) { /* Flash reads/writes should use spi_read/spi_write*/
                break;
            }

            val_mem_read_wide((uint32_t *)memory_desc->start, &data);

            if (memory_desc->mem_type == TYPE_NORMAL_READ_WRITE) {
                val_mem_write_wide((uint32_t *)memory_desc->start, data);
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
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
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
            } else if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
                return;
            }
            val_mem_read_wide((uint32_t *)soc_peripheral_desc->base, &data);

            instance++;
        } while (instance < GET_NUM_INSTANCE(soc_peripheral_desc));
        minor_id++;
        if(status != TBSA_STATUS_NOT_FOUND) {
            per_num += GET_NUM_INSTANCE(soc_peripheral_desc);
        }
    }

    status = setup_ns_env();
    if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
}

