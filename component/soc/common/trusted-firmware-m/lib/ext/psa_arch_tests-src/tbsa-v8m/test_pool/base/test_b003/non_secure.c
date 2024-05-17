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

#define PATTERN1    0x73737373
#define PATTERN2    0x56565656

TBSA_TEST_PUBLISH(CREATE_TEST_ID(TBSA_BASE_BASE, 3),
                  CREATE_TEST_TITLE("Check scrub of shared storage when reallocated from Trusted to Non-trusted"),
                  CREATE_REF_TAG("R100_TBSA_INFRA"),
                  entry_hook,
                  test_payload,
                  exit_hook);

memory_desc_t *memory_desc;

void entry_hook(tbsa_val_api_t *val)
{
    val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
}

void test_payload(tbsa_val_api_t *val)
{
    tbsa_status_t status;
    uint32_t      instance = 0;
    uint32_t      pattern1;
    uint32_t      pattern2;

    do {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_SRAM, instance),
                                        (uint8_t **)&memory_desc,
                                        (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_5, status)) {
            return;
        }

        if (memory_desc->attribute == MEM_CONFIGURABLE) {
            /* Read the previously written location and check they are scrubbed */
            val->mem_read((uint32_t*)(memory_desc->start + memory_desc->ns_offset), WORD, &pattern1);
            val->mem_read((uint32_t*)(memory_desc->end - ((uint32_t)(memory_desc->end) % 0x4) - 0x4 + memory_desc->ns_offset), WORD, &pattern2);
            if ((pattern1 != PATTERN1) && (pattern2 != PATTERN2)) {
                val->set_status(RESULT_PASS(TBSA_STATUS_SUCCESS));
            } else {
                val->err_check_set(TEST_CHECKPOINT_6, TBSA_STATUS_ERROR);
                return;
            }
        }
        instance++;
    } while(instance < GET_NUM_INSTANCE(memory_desc));
}

void exit_hook(tbsa_val_api_t *val)
{
}
