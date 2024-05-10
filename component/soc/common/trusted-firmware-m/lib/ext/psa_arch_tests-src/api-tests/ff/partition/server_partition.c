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

#include "server_partition.h"

val_api_t *val_server_sp = &val_api;
psa_api_t *psa_server_sp = &psa_api;

void server_main(void)
{
    uint32_t        test_data = 0;
    int32_t         test_status;
    psa_signal_t    signals = 0;
    val_status_t    status;
    psa_msg_t       msg = {0};
    server_test_t   *test_list;

    while (1)
    {
        status = VAL_STATUS_SUCCESS;
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & SERVER_TEST_DISPATCHER_SIGNAL)
        {
            psa_get(SERVER_TEST_DISPATCHER_SIGNAL, &msg);
            switch (msg.type)
            {
                case PSA_IPC_CONNECT:
                    if (test_data != 0)
                    {
                        val_print(PRINT_ERROR, "must clear previous dispatcher connection\n", 0);
                        psa_reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
                    }
                    else
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    break;
                case PSA_IPC_CALL:
                    if ((msg.in_size[0] <= sizeof(test_data)) &&
                        (psa_read(msg.handle, 0, &test_data, msg.in_size[0]) != sizeof(test_data)))
                    {
                        val_print(PRINT_ERROR, "could not read dispatcher payload\n", 0);
                        status = VAL_STATUS_READ_FAILED;
                    }
                    if (VAL_ERROR(status))
                    {
                        psa_reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
                        break;
                    }

                    if (GET_ACTION_NUM(test_data) == TEST_EXECUTE_FUNC)
                    {
                        psa_reply(msg.handle, PSA_SUCCESS);
                        val_print(PRINT_INFO,"\tSERVER TEST FUNC START %d\n",
                                                    GET_BLOCK_NUM(test_data));

                        /* Get server test list of given test */
                        test_list = server_ipc_test_list[GET_TEST_NUM(test_data)];

                        /* Execute server test func */
                        test_status = test_list[GET_BLOCK_NUM(test_data)]();
                    }
                    else if (GET_ACTION_NUM(test_data) == TEST_RETURN_RESULT)
                    {
                        val_print(PRINT_INFO,"\tSERVER TEST FUNC END\n", 0);
                        psa_write(msg.handle, 0, &test_status, msg.out_size[0]);
                        psa_reply(msg.handle, PSA_SUCCESS);
                    }
                    else
                    {
                        psa_reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
                    }
                    break;
                case PSA_IPC_DISCONNECT:
                    test_data=0;
                    status=0;
                    test_status=0;
                    psa_reply(msg.handle, PSA_SUCCESS);
                    break;
                default:
                    val_print(PRINT_ERROR, "Unexpected message type %d!", (int)(msg.type));
                    TEST_PANIC();
            }
        }
        else
        {
            val_print(PRINT_ERROR, "In server_partition, Control shouldn't have reach here\n", 0);
            TEST_PANIC();
        }
    }
}
