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

tbsa_val_api_t        *g_val;
dpm_desc_t            *dpm_desc;
boot_t                boot;
memory_desc_t         *nvram_desc;

#define TEST_DATA   0x12DEED21

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_DEBUG_BASE, 4),
                  CREATE_TEST_TITLE("DPM trusted functionality must implement fuse-controlled Locked State"),
                  CREATE_REF_TAG("R090_TBSA_DEBUG"),
                  entry_hook,
                  test_payload,
                  exit_hook);

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

tbsa_status_t test_env_reset(void)
{
    tbsa_status_t status;
    boot.cb = BOOT_UNKNOWN;
    status = g_val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (g_val->err_check_set(TEST_CHECKPOINT_17, status)) {
        return status;
    }
    /*Signal the debugger that the test is complete*/
    status = g_val->debug_set_status(DBG_INIT, SEQ_NEXT_TEST);
    if (g_val->err_check_set(TEST_CHECKPOINT_18, status)) {
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

tbsa_status_t test_dbg_seq_write(uint32_t addr, dbg_seq_status_t seq_status)
{
    tbsa_status_t status;
    /*Wait for the debugger to be ready and then write the data to used by debugger*/
    status = g_val->debug_get_status(DBG_WRITE);
    if (g_val->err_check_set(TEST_CHECKPOINT_13, status)) {
        return status;
    }
    g_val->mem_write((uint32_t *)dpm_desc->data_addr, WORD, (uint32_t)addr);

    status = g_val->debug_set_status(DBG_WRITE, seq_status);
    if (g_val->err_check_set(TEST_CHECKPOINT_14, status)) {
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

tbsa_status_t test_dbg_seq_read(uint32_t *addr, dbg_seq_status_t seq_status)
{
    tbsa_status_t status;
    /*Wait for the debugger to be ready and then read the data returned by debugger*/
    status = g_val->debug_get_status(DBG_READ);
    if (g_val->err_check_set(TEST_CHECKPOINT_15, status)) {
        return status;
    }
    g_val->mem_read((uint32_t *)dpm_desc->data_addr, WORD, addr);

    status = g_val->debug_set_status(DBG_READ, seq_status);
    if (g_val->err_check_set(TEST_CHECKPOINT_16, status)) {
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

uint32_t pack_data_to_save(uint32_t a, uint32_t b, uint32_t c, uint32_t d)
{
    return ((a & 0xFF) << 24) | ((b & 0xFF) << 16) | ((c & 0xFF) << 8) | (d & 0xFF);
}

void unpack_data_and_restore(uint32_t packed_data, uint32_t *a, uint32_t *b, uint32_t *c, uint32_t *d)
{
    *a = (packed_data >> 24) & 0xFF;
    *b = (packed_data >> 16) & 0xFF;
    *c = (packed_data >> 8) & 0xFF;
    *d = packed_data & 0xFF;
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data, dpm_instance, timeout, dpm_lock, reset_done=0, dpm_status;
    uint32_t      region_num = 0, instance = 0, minor_id = MEMORY_SRAM, region_num_inst;
    dpm_hdr_t     *dpm_hdr;
    memory_hdr_t  *memory_hdr;
    memory_desc_t *memory_desc;
    bool_t        dpm_not_locked;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                    (uint8_t **)&nvram_desc, (uint32_t *)sizeof(memory_desc_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
    if (val->err_check_set(TEST_CHECKPOINT_2, status)) {
        return;
    }

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, 0, 0),
                                    (uint8_t **)&dpm_hdr, (uint32_t *)sizeof(dpm_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        return;
    }

    if (boot.cb == COLD_BOOT_REQUESTED) {
        reset_done++;
    }

    if (!reset_done) {
        /* Check if DPM is present.*/
        if (!dpm_hdr->num) {
            val->print(PRINT_ERROR, "\n\t\rNo DPM present in the platform", 0);
            val->err_check_set(TEST_CHECKPOINT_4, TBSA_STATUS_NOT_FOUND);
            return;
        }

        /* Check if the debugger is connected, by checking for the init data.*/
        status = val->debug_get_status(DBG_INIT);
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            if (status == TBSA_STATUS_SKIP)
                val->set_status(RESULT_SKIP(1));
            return;
        }
    }

    /* Check the access on addresses controlled by locked DPM */
    for (dpm_instance = 0; dpm_instance < dpm_hdr->num; dpm_instance++) {
        /*Re-initialise dpm_instance if reset was done*/
        if(reset_done) {
            status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_DPM2), &dpm_instance, sizeof(uint32_t));
            if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
                goto clean_up;
            }

        }
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, dpm_instance),
                                        (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
            goto clean_up;
        }

        /* Get the current state of the DPM under check*/
        status = val->dpm_get_state(dpm_desc->index, &dpm_status);
        if (val->err_check_set(TEST_CHECKPOINT_8, status)) {
            goto clean_up;
        }

        /* if condition - if DPM lock is implemented and lock is set, then proceed with access check.
                            if there is only one DPM and it is in locked state then exit the test, as
                            access check is not possible.
           else if cond - if DPM lock is implemented but not locked, then set a variable which will be
                          used to set the DPM lock
           else cond    - if DPM lock in not implemented then start the loop again to find next DPM with
                          lock implemented.
        */
        dpm_not_locked = FALSE;
        dpm_lock = DPM_LOCK_IMPLEMENTED|DPM_LOCK_STATE;
        if ((dpm_status & dpm_lock) == dpm_lock) {
            if (dpm_hdr->num == 1) {
                val->print(PRINT_WARN, "\n\t\rThe only DPM in the system is in locked state, cannot check access.", 0);
                val->set_status(RESULT_SKIP(1));
                goto clean_up;
            }
        } else if ((dpm_status & DPM_LOCK_IMPLEMENTED) == DPM_LOCK_IMPLEMENTED) {
            dpm_not_locked = TRUE;
        } else
            continue;

        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, 0, 0),
                              (uint8_t **)&memory_hdr, (uint32_t *)sizeof(memory_hdr_t));
        if (val->err_check_set(TEST_CHECKPOINT_9, status)) {
            goto clean_up;
        }
        /*Check for R/W address controlled by DPM under check, and access*/
        for (region_num = 0; region_num < memory_hdr->num;) {
            instance = 0;
            /* If reset was done then over-write loop variables from stored NV RAM*/
            if(reset_done) {
                status = val->nvram_read(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_DPM1), &data, sizeof(uint32_t));
                if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
                    goto clean_up;
                }
                unpack_data_and_restore(data, &region_num, &minor_id, &instance, &region_num_inst);
            }

            do {
                status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, minor_id, instance),
                                      (uint8_t **)&memory_desc, (uint32_t *)sizeof(memory_desc_t));
                if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
                    goto clean_up;
                }
                if(!reset_done)
                    region_num_inst = GET_NUM_INSTANCE(memory_desc);

                if ((memory_desc->dpm_index == dpm_desc->index) && (memory_desc->mem_type == TYPE_NORMAL_READ_WRITE)) {
                    if (!reset_done) {
                        /*Initialize the memory with known data*/
                        val->mem_write((uint32_t *)memory_desc->start, WORD, TEST_DATA);

                        if (test_dbg_seq_write((uint32_t)(memory_desc->start), SEQ_LOCKED_STATE_READ))
                            goto clean_up;

                        /* Reset will only be triggered in case if DPM was not locked at reset*/
                        if (dpm_not_locked) {
                            /* Set the DPM state to Locked.*/
                            status = val->dpm_set_state(dpm_desc, DPM_LOCKED_STATE);
                            if (val->err_check_set(TEST_CHECKPOINT_C, status)) {
                                goto clean_up;
                            }
                            /* Wait until debugger has completed the access before DPM is put to Locked state.*/
                            timeout = 0x1000000;
                            while(--timeout);

                            boot.cb = COLD_BOOT_REQUESTED;
                            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
                            if (val->err_check_set(TEST_CHECKPOINT_D, status)) {
                                goto clean_up;
                            }
                            data =  pack_data_to_save(region_num, minor_id, instance, region_num_inst);
                            status = val->nvram_write(nvram_desc->start,  TBSA_NVRAM_OFFSET(NV_DPM1), &data, sizeof(uint32_t));
                            if (val->err_check_set(TEST_CHECKPOINT_E, status)) {
                                goto clean_up;
                            }
                            status = val->nvram_write(nvram_desc->start, TBSA_NVRAM_OFFSET(NV_DPM2), &dpm_instance, sizeof(uint32_t));
                            if (val->err_check_set(TEST_CHECKPOINT_F, status)) {
                                goto clean_up;
                            }
                            val->system_reset(COLD_RESET);
                            /* shouldn't come here */
                            if (val->err_check_set(TEST_CHECKPOINT_10, TBSA_STATUS_ERROR)) {
                                goto clean_up;
                            }
                        } else {
                            /* Wait until debugger has completed the access when DPM is in Locked state.*/
                            timeout = 0x1000000;
                            while(--timeout);
                        }
                    }

                    /* Read the data returned by debugger and compare to get the results.*/
                    if(test_dbg_seq_read(&data, SEQ_LOCKED_STATE_READ))
                        goto clean_up;

                    if (data == TEST_DATA) {
                        val->err_check_set(TEST_CHECKPOINT_11, TBSA_STATUS_ERROR);
                        val->print(PRINT_ERROR, "\n\t\rDPM could not restrict access in Locked State", 0);
                        val->print(PRINT_ERROR, "\n\t\rDebugger read the actual data = 0x%x", TEST_DATA);
                        val->print(PRINT_ERROR, " at address = 0x%x", (uint32_t)(memory_desc->start));
                        goto clean_up;
                    }
                }
                instance++;
            } while (instance < GET_NUM_INSTANCE(memory_desc));
            minor_id++;
            region_num += GET_NUM_INSTANCE(memory_desc);
        }
        reset_done = 0;
    }

    if (test_env_reset())
    return;

    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    return;

clean_up:

    if (test_env_reset())
    return;
}

void exit_hook(tbsa_val_api_t *val)
{
}
