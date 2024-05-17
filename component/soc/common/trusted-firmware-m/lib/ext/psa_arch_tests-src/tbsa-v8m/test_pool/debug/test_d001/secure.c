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

#define TEST_DATA     0x12DEED21

tbsa_val_api_t        *g_val;
dpm_desc_t            *dpm_desc;

/*  Publish these functions to the external world as associated to this test ID */
TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_DEBUG_BASE, 1),
                  CREATE_TEST_TITLE("There must be a DPM to permit access to all assets"),
                  CREATE_REF_TAG("R010/R030/R120_TBSA_DEBUG"),
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

tbsa_status_t test_dbg_seq_write(uint32_t addr, dbg_seq_status_t seq_status)
{
    tbsa_status_t status;
    /*Wait for the debugger to be ready and then write the data to used by debugger*/
    status = g_val->debug_get_status(DBG_WRITE);
    if (g_val->err_check_set(TEST_CHECKPOINT_C, status)) {
        return status;
    }
    g_val->mem_write((uint32_t *)dpm_desc->data_addr, WORD, addr);

    status = g_val->debug_set_status(DBG_WRITE, seq_status);
    if (g_val->err_check_set(TEST_CHECKPOINT_D, status)) {
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

tbsa_status_t test_dbg_seq_read(uint32_t *addr, dbg_seq_status_t seq_status)
{
    tbsa_status_t status;
    /*Wait for the debugger to be ready and then read the data returned by debugger*/
    status = g_val->debug_get_status(DBG_READ);
    if (g_val->err_check_set(TEST_CHECKPOINT_E, status)) {
        return status;
    }
    g_val->mem_read((uint32_t *)dpm_desc->data_addr, WORD, addr);

    status = g_val->debug_set_status(DBG_READ, seq_status);
    if (g_val->err_check_set(TEST_CHECKPOINT_F, status)) {
        return status;
    }
    return TBSA_STATUS_SUCCESS;
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      data, dpm_instance;
    uint32_t      region_num = 0, instance = 0, minor_id = MEMORY_SRAM;
    dpm_hdr_t     *dpm_hdr;
    memory_hdr_t  *memory_hdr;
    memory_desc_t *memory_desc;

    status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, 0, 0),
                                    (uint8_t **)&dpm_hdr, (uint32_t *)sizeof(dpm_hdr_t));
    if (val->err_check_set(TEST_CHECKPOINT_1, status)) {
        return;
    }

    /* Check if DPM is present.*/
    if (!dpm_hdr->num) {
        val->print(PRINT_ERROR, "\n\r\tNo DPM present in the platform", 0);
        val->err_check_set(TEST_CHECKPOINT_2, TBSA_STATUS_NOT_FOUND);
        return;
    }

    /* Check if the debugger is connected, by checking for the init data.*/
    status = val->debug_get_status(DBG_INIT);
    if (val->err_check_set(TEST_CHECKPOINT_3, status)) {
        if (status == TBSA_STATUS_SKIP)
            val->set_status(RESULT_SKIP(1));
        return;
    }

    for (dpm_instance = 0; dpm_instance < dpm_hdr->num; dpm_instance++) {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_DPM, DPM_DPM, dpm_instance),
                                        (uint8_t **)&dpm_desc, (uint32_t *)sizeof(dpm_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_4, status)) {
            goto clean_up;
        }

        /* Set the DPM state to Open.*/
        status = val->dpm_set_state(dpm_desc, DPM_OPEN_STATE);
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            goto clean_up;
        }

        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, 0, 0),
                              (uint8_t **)&memory_hdr, (uint32_t *)sizeof(memory_hdr_t));
        if (val->err_check_set(TEST_CHECKPOINT_6, status)) {
            goto clean_up;
        }
        /*Check for R/W address controlled by DPM under check, and access*/
        for (region_num = 0; region_num < memory_hdr->num;) {
            instance = 0;
            do {
                status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, minor_id, instance),
                                      (uint8_t **)&memory_desc, (uint32_t *)sizeof(memory_desc_t));
                if (val->err_check_set(TEST_CHECKPOINT_7, status)) {
                    goto clean_up;
                }

                if ((memory_desc->dpm_index == dpm_desc->index) && (memory_desc->mem_type == TYPE_NORMAL_READ_WRITE)) {

                    /*Initialize the memory with known data*/
                    val->mem_write((uint32_t *)memory_desc->start, WORD, TEST_DATA);

                    if (test_dbg_seq_write((uint32_t)(memory_desc->start), SEQ_OPEN_STATE_READ))
                        goto clean_up;

                    /* Read the data returned by debugger and compare to get the results.*/
                    if(test_dbg_seq_read(&data, SEQ_OPEN_STATE_READ))
                        goto clean_up;

                    if (data != TEST_DATA) {
                        val->err_check_set(TEST_CHECKPOINT_8, TBSA_STATUS_ERROR);
                        val->print(PRINT_ERROR, "\n\r\tIncorrect data returned by debugger, actual = 0x%x", data);
                        val->print(PRINT_ERROR, " expected = 0x%x", TEST_DATA);
                        goto clean_up;
                    }

                    /*Initialize the memory with known data*/
                    val->mem_write((uint32_t *)memory_desc->start, WORD, ~TEST_DATA);

                    if (test_dbg_seq_write((uint32_t)(memory_desc->start), SEQ_OPEN_STATE_WRITE)) {
                        goto clean_up;
                    }

                    if (test_dbg_seq_write(TEST_DATA, SEQ_OPEN_STATE_WRITE)) {
                        goto clean_up;
                    }

                    val->mem_read((uint32_t *)memory_desc->start, WORD, &data);

                    if (data != TEST_DATA) {
                        val->err_check_set(TEST_CHECKPOINT_9, TBSA_STATUS_ERROR);
                        val->print(PRINT_ERROR, "\n\r\tIncorrect data returned by debugger, actual = 0x%x", data);
                        val->print(PRINT_ERROR, " expected = 0x%x", TEST_DATA);
                        goto clean_up;
                    }
                }
                instance++;
            } while (instance < GET_NUM_INSTANCE(memory_desc));
            minor_id++;
            region_num += GET_NUM_INSTANCE(memory_desc);
        }
    }

    /*Signal the debugger that the test is complete*/
    status = val->debug_set_status(DBG_INIT, SEQ_NEXT_TEST);
    if (val->err_check_set(TEST_CHECKPOINT_A, status)) {
        return;
    }
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
    return;

clean_up:
    /*Signal the debugger that the test is complete*/
    status = val->debug_set_status(DBG_INIT, SEQ_NEXT_TEST);
    if (val->err_check_set(TEST_CHECKPOINT_B, status)) {
        return;
    }
}

void exit_hook(tbsa_val_api_t *val)
{
}
