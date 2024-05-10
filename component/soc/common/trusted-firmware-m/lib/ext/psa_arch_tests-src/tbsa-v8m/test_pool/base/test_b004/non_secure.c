/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 4),
                  CREATE_TEST_TITLE("Check memory must not be executable/NSC after it is reallocated from NT to T"),
                  CREATE_REF_TAG("R110/R230_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t *memory_desc;
memory_desc_t *nvram_desc;

uint32_t read_cpuid(void)
{
    uint32_t cpuid_ns;

    /* Reading CPUID_NS */
    cpuid_ns = *(uint32_t*)CPUID_NS;

    return cpuid_ns;
}

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      instance         = 0UL;
    bool_t        sram_block_found = FALSE;
    boot_t        boot;
    uint32_t      shcsr            = 0UL;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&nvram_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    /* Disabling SecureFault, UsageFault, BusFault, MemFault temporarily */
    status = val->mem_reg_read(SHCSR, &shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
        return;
    }

    status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
    if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
        return;
    }

    status = val->mem_reg_write(SHCSR, (shcsr & ~0xF0000));
    if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
        return;
    }

    if (boot.wb != WARM_BOOT_REQUESTED) {

        do {
            status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_SRAM, instance),
                                            (uint8_t **)&memory_desc,
                                            (uint32_t *)sizeof(memory_desc_t));
            if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                return;
            }

            if (memory_desc->attribute == MEM_CONFIGURABLE) {
                /* We found a configurable SRAM block */
                sram_block_found = TRUE;

                /* Ensure the configurable block is Non-secure */
                status = val->mpc_configure_security_attribute(memory_desc->mpc, \
                                                               (memory_desc->start + memory_desc->ns_offset), \
                                                               (memory_desc->end + memory_desc->ns_offset), \
                                                               MEM_NONSECURE);
                if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
                    return;
                }

                /* Let's run a piece of code in Non-secure mode which returns value zero */
                if (read_cpuid()) {
                    /* Flag incorrect value error */
                    val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_INCORRECT_VALUE);
                    return;
                }

                /* Ensure we have sufficient space to run a piece of code in trusted mode */
                if ((memory_desc->end - memory_desc->start) < 0x20 ) {
                    val->err_check_set(TEST_CHECKPOINT_A, TBSA_STATUS_INSUFFICIENT_SIZE);
                    return;
                }

                /* Configure a given block of memory as Secure one, before that */
                /* let's copy opcodes which shall have valid results when executed from trusted world */
                val->memcpy((void*)(memory_desc->start + memory_desc->ns_offset), (void*)((uint32_t)read_cpuid & ~0x1UL), 0x20);

                status = val->mpc_configure_security_attribute(memory_desc->mpc, \
                                                               (memory_desc->start + memory_desc->s_offset), \
                                                               (memory_desc->end + memory_desc->s_offset), \
                                                               MEM_SECURE);
                if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                    return;
                }

                break;
            }
            instance++;
        } while(instance < GET_NUM_INSTANCE(memory_desc));

        if (!sram_block_found) {
            val->err_check_set(TEST_CHECKPOINT_C, TBSA_STATUS_NOT_FOUND);
            return;
        }

        boot.wb = WARM_BOOT_REQUESTED;
        status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
        if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
            return;
        }

        /* VAL call to execute code in secure mode */
        /* As per the rule, we cannot execute in secure mode as the memory block is reallocated from
         * Non-trusted to trusted, so we expect fault to occur !
         */
        val->execute_in_trusted_mode(memory_desc->start + memory_desc->s_offset);
        /* Shouldn't come here */
        val->err_check_set(TEST_CHECKPOINT_E, TBSA_STATUS_ERROR);
        return;
    }

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void exit_hook(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    boot_t        boot;
    uint32_t      shcsr = 0UL;

    /* If we are here means, we are in second run of this test */
    boot.wb = BOOT_UNKNOWN;
    status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return;
    }

    status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_SHCSR), &shcsr, sizeof(shcsr));
    if (val->err_check_set(TEST_CHECKPOINT_10, status)) {
        return;
    }

    /* Restoring faults */
    status = val->mem_reg_write(SHCSR, shcsr);
    if (val->err_check_set(TEST_CHECKPOINT_11, status)) {
        return;
    }

    /* Restoring default Handler */
    status = val->interrupt_restore_handler(EXCP_NUM_HF);
    if (val->err_check_set(TEST_CHECKPOINT_12, status)) {
        return;
    }
}
