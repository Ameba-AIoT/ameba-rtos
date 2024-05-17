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

#include "val_client_defs.h"
#include "val_service_defs.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

int32_t server_test_psa_drop_connection(void);

const server_test_t test_i027_server_tests_list[] = {
    NULL,
    server_test_psa_drop_connection,
    NULL,
};

int32_t server_test_psa_drop_connection(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    status = val->process_connect_request(SERVER_CONNECTION_DROP_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_CONNECTION_DROP_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, -2);
        goto exit;
    }
    else
    {
        psa->reply(msg.handle, PSA_ERROR_PROGRAMMER_ERROR);
    }

exit:
    /*
     * SPM must deliver a PSA_IPC_DISCONNECT message for the connection to the RoT Service
     * directly after receipt of the PSA_ERROR_PROGRAMMER_ERROR completion to allow
     * connection resources within the RoT Service to be released
     */
    status = val->process_disconnect_request(SERVER_CONNECTION_DROP_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        val->print(PRINT_ERROR, "\tDisconnect request failed\n", 0);
    }
    psa->reply(msg.handle, PSA_SUCCESS);
    return status;
}
