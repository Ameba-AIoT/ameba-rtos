/** @file
 * Copyright (c) 2021, Arm Limited or its affiliates. All rights reserved.
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

#include <psa_adac.h>
#include <adac_util.h>
#include "val_interfaces.h"

#include "test_a007.h"

#define TEST_NUM  VAL_CREATE_TEST_ID(VAL_SECURE_DEBUG_BASE, 7)
#define TEST_DESC "Testing ADAC Protocol Host API| UT: psa_invalid_command\n"
TEST_PUBLISH(TEST_NUM, test_entry);

void test_entry(val_api_t *val_api)
{
    psa_status_t ret;
    val_api_t *val = val_api;

    /* test init */
    val->test_init(TEST_NUM, TEST_DESC);
    if (!IS_TEST_START(val->get_status()))
    {
        goto test_end;
    }
    psa_adac_host_init();

    request_packet_t *request;
    response_packet_t *response;
    uint16_t command[] = { 0x0, 0x100, 0xBAD, 0x7FFF};
    uint32_t i, cmd_list_size;

    cmd_list_size = sizeof(command)/sizeof(command[0]);

    for (i = 0; i < cmd_list_size; i++) {
        request = request_packet_build(command[i], NULL, 0);
        if (request != NULL) {
	        val->print(PRINT_INFO, "Sending command 0x%x\n", command[i]);
            if (request_packet_send(request) < 0) {
                val->err_check_set(TEST_CHECKPOINT_NUM((1*i)), VAL_STATUS_WRITE_FAILED);
                goto test_end;
            }
            request_packet_release(request);
        }

        response = response_packet_receive();
        if (response->status != SDP_INVALID_COMMAND)
            val->err_check_set(TEST_CHECKPOINT_NUM((2*i)), VAL_STATUS_ERROR);

        response_packet_release(response);
    }

    if (i != cmd_list_size)
        val->err_check_set(TEST_CHECKPOINT_NUM((2*i-1)), VAL_STATUS_ERROR);

test_end:
    val->test_exit();
}
