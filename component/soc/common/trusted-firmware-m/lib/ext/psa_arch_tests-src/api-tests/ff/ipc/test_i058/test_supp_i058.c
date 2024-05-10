/** @file
 * Copyright (c) 2019-2022, Arm Limited or its affiliates. All rights reserved.
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

#if STATELESS_ROT == 1

int32_t server_test_psa_doorbell_signal(void);

const server_test_t test_i058_server_tests_list[] = {
    NULL,
    server_test_psa_doorbell_signal,
    NULL,
};

int32_t server_test_psa_doorbell_signal(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    val->err_check_set(TEST_CHECKPOINT_NUM(201), status);
    psa_reply(msg.handle, PSA_SUCCESS);

    if (msg.client_id > 0)
    {
        /* Doorbell signal to client partititon */
        psa->notify(msg.client_id);
    }
    else
    {
        status = VAL_STATUS_SPM_FAILED;
        val->print(PRINT_ERROR, "Caller is from non-secure\n", 0);
    }

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    val->err_check_set(TEST_CHECKPOINT_NUM(202), status);
    psa_reply(msg.handle, PSA_SUCCESS);
    return status;
}

#else

int32_t server_test_psa_doorbell_signal(void);

const server_test_t test_i058_server_tests_list[] = {
    NULL,
    server_test_psa_doorbell_signal,
    NULL,
};

int32_t server_test_psa_doorbell_signal(void)
{
    int32_t                 status = VAL_STATUS_SUCCESS;
    psa_msg_t               msg = {0};

    /* Serve psa_connect */
    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /* Accept the connection */
    psa->reply(msg.handle, PSA_SUCCESS);

    if (msg.client_id > 0)
    {
        /* Doorbell signal to client partititon */
        psa->notify(msg.client_id);
    }
    else
    {
        status = VAL_STATUS_SPM_FAILED;
        val->print(PRINT_ERROR, "Caller is from non-secure\n", 0);
    }

    /* Serve psa_close */
    status = ((val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
               ? VAL_STATUS_ERROR : status);
    val->err_check_set(TEST_CHECKPOINT_NUM(202), status);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

#endif