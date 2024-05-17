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
#include "val_infra.h"
#include "val_test.h"
#include "val_secure.h"
#include "val_test_common.h"

extern tbsa_val_api_t tbsa_val_s_api;
extern tbsa_val_api_t tbsa_val_ns_api;
extern addr_t g_s_test_info_addr;
extern addr_t g_ns_test_info_addr;

/**
    @brief    - Execute the function pointer which was given to us by the test
    @param    - test_id      : Current test ID
              - test_fn_type : Test function type
    @return   - void
**/
void val_execute_test_fn(test_id_t test_id, tbsa_test_fn_type_t test_fn_type)
{
    test_fptr_t   fn_ptr;
    addr_t        addr;

    val_test_get_info(test_id, test_fn_type, &addr);

    switch(test_fn_type)
    {
        case ENTRY_FUNC_HOOK_S:
        case PAYLOAD_FUNC_S:
        case EXIT_FUNC_HOOK_S:
            fn_ptr = (test_fptr_t)addr;
            fn_ptr(&tbsa_val_s_api);
            break;
        case ENTRY_FUNC_HOOK_NS:
        case PAYLOAD_FUNC_NS:
        case EXIT_FUNC_HOOK_NS:
                val_s_to_ns(addr, &tbsa_val_ns_api);
            break;
    }

    return;
}

/**
    @brief    - This function is responsible for setting up VAL infrastructure.
                Loads TBSA tests one by one and calls the functions(entry/payload/exit) from each test image.
    @param    - test_id_prev  : Previous ran test ID
    @return   - none
**/
void tbsa_dispatcher(test_id_t test_id_prev)
{

    test_id_t     test_id;
    tbsa_status_t status;
    memory_desc_t *memory_desc;
    test_count_t  test_count;

    status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVRAM, 0),
                                   (uint8_t **)&memory_desc,
                                   (uint32_t *)sizeof(memory_desc_t));
    if(status != TBSA_STATUS_SUCCESS) {
        val_print(PRINT_ERROR, "\n\tNVRAM not present", 0);
        goto exit;
    }

    do
    {
        status = val_test_load(&test_id, test_id_prev);
        if (test_id == TBSA_TEST_INVALID) {
            status = val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST), &test_id, sizeof(test_id_t));
            if(status != TBSA_STATUS_SUCCESS) {
                val_print(PRINT_ALWAYS, "\n\tNVRAM write error", 0);
            }
            break;
        }
        if (TBSA_ERROR(status))
        {
            continue;
        }

        if (TBSA_GET_COMP_NUM(test_id_prev) != TBSA_GET_COMP_NUM(test_id)) {
            val_print(PRINT_ALWAYS, val_get_comp_name(test_id), 0);
        }
        val_print(PRINT_ALWAYS, "\n\n\r%d : ", test_id);
        val_print(PRINT_ALWAYS, ((tbsa_test_info_t *)g_s_test_info_addr)->ref_tag, 0);
        val_print(PRINT_ALWAYS, "\r      ", 0);
        val_print(PRINT_ALWAYS, ((tbsa_test_info_t *)g_s_test_info_addr)->title, 0);
        val_print(PRINT_ALWAYS, "\r      # Secure    ", 0);

        val_execute_test_fn(test_id, ENTRY_FUNC_HOOK_S);

        if (IS_TEST_PASS(val_get_status()))
        {
            val_execute_test_fn(test_id, PAYLOAD_FUNC_S);
        }

        /* calling S exit hook unconditionally */
        val_execute_test_fn(test_id, EXIT_FUNC_HOOK_S);

        val_report_status(test_id);
        val_print(PRINT_ALWAYS, "\n\r      # Non Secure", 0);

        if (IS_TEST_PASS(val_get_status()))
        {
            val_execute_test_fn(test_id, ENTRY_FUNC_HOOK_NS);
        }

        if (IS_TEST_PASS(val_get_status()))
        {
            val_execute_test_fn(test_id, PAYLOAD_FUNC_NS);
        }

        /* calling NS exit hook unconditionally */
        val_execute_test_fn(test_id, EXIT_FUNC_HOOK_NS);

        status = val_nvram_read(memory_desc->start,
                                TBSA_NVRAM_OFFSET(NV_TEST_CNT),
                                &test_count,
                                sizeof(test_count_t));
        if(status != TBSA_STATUS_SUCCESS) {
            val_print(PRINT_ALWAYS, "\n\tNVRAM write error", 0);
            break;
        }
        if (IS_TEST_PASS(val_get_status())) {
            test_count.pass_cnt += 1;
        } else if(IS_TEST_SKIP(val_get_status())) {
            test_count.skip_cnt += 1;
        } else if(IS_TEST_FAIL(val_get_status())) {
            test_count.fail_cnt += 1;
        }
        status = val_nvram_write(memory_desc->start,
                                 TBSA_NVRAM_OFFSET(NV_TEST_CNT),
                                 &test_count,
                                 sizeof(test_count_t));
        if(status != TBSA_STATUS_SUCCESS) {
            val_print(PRINT_ALWAYS, "\n\tNVRAM write error", 0);
            break;
        }

        val_report_status(test_id);

        test_id_prev = test_id;

        status = val_nvram_write(memory_desc->start, TBSA_NVRAM_OFFSET(NV_TEST), &test_id_prev, sizeof(test_id_t));
        if(status != TBSA_STATUS_SUCCESS) {
            val_print(PRINT_ALWAYS, "\n\tNVRAM write error", 0);
            break;
        }

    } while(1);

exit:
   val_infra_exit();
}
