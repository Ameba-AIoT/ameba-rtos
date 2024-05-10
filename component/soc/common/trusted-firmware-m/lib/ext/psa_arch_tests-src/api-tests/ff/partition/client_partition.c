/** @file
 * Copyright (c) 2018-2022, Arm Limited or its affiliates. All rights reserved.
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

#include "client_partition.h"
val_api_t *val_client_sp = &val_api;
psa_api_t *psa_client_sp = &psa_api;

void client_main(void)
{
    uint32_t        test_data = 0;
    psa_signal_t    signals = 0;
    val_status_t    status, test_status;
    psa_msg_t       msg = {0};
    test_info_t     test_info;

    while (1)
    {
        status = VAL_STATUS_SUCCESS;
        signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);

        if (signals & CLIENT_TEST_DISPATCHER_SIGNAL)
        {
            psa_get(CLIENT_TEST_DISPATCHER_SIGNAL, &msg);
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

                        val_print(PRINT_INFO, "In client part, test-id %d\n",
                                                    GET_TEST_NUM(test_data));
                        test_info.test_num = GET_TEST_NUM(test_data);
                        test_info.block_num = GET_BLOCK_NUM(test_data);
                        /* Execute client test func from secure*/
                        test_status = val_execute_secure_tests(test_info,
                                            client_ipc_test_list[GET_TEST_NUM(test_data)]);
                    }
                    else if (GET_ACTION_NUM(test_data) == TEST_RETURN_RESULT)
                    {
                        psa_write(msg.handle, 0, &test_status, sizeof(test_status));
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
        /* Server_partition requests client to connect to SERVER_SECURE_CONNECT_ONLY_SID */
        else if (signals & PSA_DOORBELL)
        {
#if STATELESS_ROT == 1
            psa_call(SERVER_SECURE_CONNECT_ONLY_HANDLE, PSA_IPC_CALL, NULL, 0, NULL, 0);
#else
            if (psa_connect(SERVER_SECURE_CONNECT_ONLY_SID, SERVER_SECURE_CONNECT_ONLY_VERSION)
                != PSA_ERROR_CONNECTION_REFUSED)
            {
               val_print(PRINT_ERROR, "psa_connect failed \n", 0);
            }
#endif
            psa_clear();
        }
        else
        {
            val_print(PRINT_ERROR, "In client_partition, Control shouldn't have reach here\n", 0);
            TEST_PANIC();
        }
    }
}
