/** @file
 * Copyright (c) 2019-2021, Arm Limited or its affiliates. All rights reserved.
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
#include "stdio.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

#if STATELESS_ROT == 1

int32_t server_test_psa_call_with_invalid_outvec_pointer(void);

const server_test_t test_i049_server_tests_list[] = {
    NULL,
    server_test_psa_call_with_invalid_outvec_pointer,
    NULL,
};

int32_t server_test_psa_call_with_invalid_outvec_pointer(void)
{
    psa_msg_t       msg = {0};
    psa_signal_t    signals;

wait:
    signals = psa->wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
    {
    	goto wait;
    }

    if (msg.type == PSA_IPC_CALL)
    {
    	/* Control shouldn't have come here */
        val->print(PRINT_ERROR, "\tControl shouldn't have reached here\n", 0);
        psa->reply(msg.handle, -2);
    }

    return VAL_STATUS_ERROR;
}

#else

int32_t server_test_psa_call_with_invalid_outvec_pointer(void);

const server_test_t test_i049_server_tests_list[] = {
    NULL,
    server_test_psa_call_with_invalid_outvec_pointer,
    NULL,
};

int32_t server_test_psa_call_with_invalid_outvec_pointer(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    psa_signal_t    signals;

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    psa->reply(msg.handle, PSA_SUCCESS);

wait:
    signals = psa->wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (signals & SERVER_UNSPECIFED_VERSION_SIGNAL)
    {
        if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
        {
            goto wait;
        }

        if (msg.type == PSA_IPC_CALL)
        {
            /* Control shouldn't have come here */
            val->print(PRINT_ERROR, "\tControl shouldn't have reached here\n", 0);
            psa->reply(msg.handle, -2);
            val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
            psa->reply(msg.handle, PSA_SUCCESS);
        }
        else if (msg.type == PSA_IPC_DISCONNECT)
        {
            psa->reply(msg.handle, PSA_SUCCESS);
            return VAL_STATUS_SUCCESS;
        }
    }
    else
    {
        val->print(PRINT_ERROR, "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
        return VAL_STATUS_ERROR;
    }

    return VAL_STATUS_ERROR;
}

#endif
