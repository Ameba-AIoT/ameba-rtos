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

#define CONNECT_NUM 2

int32_t server_test_connection_busy_and_reject(void);
int32_t server_test_accept_and_close_connect(void);
int32_t server_test_connect_with_allowed_version_policy(void);
int32_t server_test_psa_call_with_allowed_status_code(void);
int32_t server_test_psa_call_with_allowed_type_values(void);
int32_t server_test_identity(void);
int32_t server_test_spm_concurrent_connect_limit(void);
int32_t server_test_psa_block_behave(void);
int32_t server_test_psa_poll_behave(void);

const server_test_t test_i002_server_tests_list[] = {
    NULL,
    server_test_connection_busy_and_reject,
    server_test_accept_and_close_connect,
    server_test_connect_with_allowed_version_policy,
    server_test_psa_call_with_allowed_status_code,
    server_test_psa_call_with_allowed_type_values,
    server_test_identity,
    server_test_spm_concurrent_connect_limit,
    server_test_psa_block_behave,
    server_test_psa_poll_behave,
    NULL,
};

int32_t server_test_connection_busy_and_reject(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};

    /*
     * Checks performed:
     * psa->wait()- Returns > 0 when at least one signal is asserted
     * check delivery of PSA_IPC_CONNECT when psa_connect called.
     * And msg.handle must be positive.
    */
    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        return status;
    }

    /* Rejecting connection to check behaviour of PSA_ERROR_CONNECTION_BUSY */
    psa->reply(msg.handle, PSA_ERROR_CONNECTION_BUSY);

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        return status;
    }

    /* Rejecting connection to check behaviour of PSA_ERROR_CONNECTION_REFUSED */
    psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
    return status;
}

int32_t server_test_accept_and_close_connect(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        /* Reject the connection if processing of connect request has failed */
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /* Accepting connection to check behaviour of PSA_SUCCESS */
    psa->reply(msg.handle, PSA_SUCCESS);

    /* Checking delivery of PSA_IPC_DISCONNECT when psa_close called
     * msg.handle must be positive
     */
    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(204), status))
    {
        return status;
    }

    /* Sanity check - if the message type is PSA_IPC_DISCONNECT then the status code is ignored.*/
    psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);

    /* Debug print for sanity check */
    val->err_check_set(TEST_CHECKPOINT_NUM(205), status);
    return status;
}

int32_t server_test_connect_with_allowed_version_policy(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    int             i = 0;
    psa_signal_t    signal[4] = {SERVER_UNSPECIFED_VERSION_SIGNAL,
                                 SERVER_STRICT_VERSION_SIGNAL,
                                 SERVER_RELAX_VERSION_SIGNAL,
                                 SERVER_RELAX_VERSION_SIGNAL};

    for (i = 0; i < 4; i++)
    {
        status = ((val->process_connect_request(signal[i], &msg))
                                    ? VAL_STATUS_ERROR : status);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(206), status))
        {
            psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
            return status;
        }

        psa->reply(msg.handle, PSA_SUCCESS);

        status = ((val->process_disconnect_request(signal[i], &msg))
                                        ? VAL_STATUS_ERROR : status);
        val->err_check_set(TEST_CHECKPOINT_NUM(207), status);
        psa->reply(msg.handle, PSA_SUCCESS);
    }
    return status;
}

int32_t server_test_psa_call_with_allowed_status_code(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    uint32_t        i = 0;
    psa_status_t    status_code[] = {PSA_SUCCESS, 1, 2, INT32_MAX, -1, -2, INT32_MIN+128};

    for (i = 0; i < (sizeof(status_code)/sizeof(status_code[0])); i++)
    {
        status = ((val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
                                        ? VAL_STATUS_ERROR : status);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(208), status))
        {
            psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
            return status;
        }
        psa->reply(msg.handle, PSA_SUCCESS);

        status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(209), status))
        {
            /* Send status code other than status_code[] to indicate failure
             * in processing call request
             */
            psa->reply(msg.handle, -3);
        }
        else
        {
            /* Send various status code available in status_code[] */
            psa->reply(msg.handle, status_code[i]);
        }

        status = ((val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
                                        ? VAL_STATUS_ERROR : status);
        psa->reply(msg.handle, PSA_SUCCESS);
        val->err_check_set(TEST_CHECKPOINT_NUM(210), status);
    }
    return status;
}

int32_t server_test_psa_call_with_allowed_type_values(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};
    int16_t         type[] = {PSA_IPC_CALL, 1, 2, INT16_MAX};
    uint32_t        i = 0;

    status = ((val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
                                    ? VAL_STATUS_ERROR : status);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(211), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    for (i = 0; i < (sizeof(type)/sizeof(type[0])); i++)
    {
        status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
        if (val->err_check_set(TEST_CHECKPOINT_NUM(212), status))
        {
            psa->reply(msg.handle, -3);
            break;
        }
        else
        {
            /* Check recieve of client provided message type */
            if (msg.type != type[i])
            {
                status = VAL_STATUS_CALL_FAILED;
                psa->reply(msg.handle, -2);
                break;
            }
            psa->reply(msg.handle, PSA_SUCCESS);
        }
    }

    status = ((val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
                                    ? VAL_STATUS_ERROR : status);
    psa->reply(msg.handle, PSA_SUCCESS);
    val->err_check_set(TEST_CHECKPOINT_NUM(213), status);
    return status;
}

int32_t server_test_identity(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    psa_msg_t   msg = {0};
    int         id_at_connect = 0, id_at_call = 0;

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);

    if (val->err_check_set(TEST_CHECKPOINT_NUM(214), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /* Client ID during connect */
    id_at_connect = msg.client_id;
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(215), status))
    {
        psa->reply(msg.handle, -3);
    }
    else
    {
        /* Client ID during call */
        id_at_call = msg.client_id;
        psa->write(msg.handle, 0, &id_at_connect, msg.out_size[0]);
        psa->write(msg.handle, 1, &id_at_call, msg.out_size[1]);
        psa->reply(msg.handle, PSA_SUCCESS);
    }

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    val->err_check_set(TEST_CHECKPOINT_NUM(216), status);
    /* Client ID during disconnect. It should be equal to id_at_call */
    if (msg.client_id != id_at_call)
    {
        val->print(PRINT_ERROR, "\tmsg.client_id failed for IPC_DISCONNECT", 0);
        status = VAL_STATUS_WRONG_IDENTITY;
    }

    /* A Partition must have an alphanumeric name for source code to directly refer
     * to a specific Partition. The Secure Partition ID can be referenced in
     * Secure Partition source code via the symbolic name specified as name attribute.
     * The symbol must have the value of the Secure Partition ID.
     * Using CLIENT_PARTITION define to cover this rule.
     */
    if ((msg.client_id > 0) && (msg.client_id != CLIENT_PARTITION))
    {
        val->print(PRINT_ERROR, "\tmsg.client_id failed for CLIENT_PARTITION", 0);
        status = VAL_STATUS_WRONG_IDENTITY;
    }
    psa->reply(msg.handle, PSA_SUCCESS);
    return status;
}

int32_t server_test_spm_concurrent_connect_limit(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             count = 0;
    int32_t         status = VAL_STATUS_SUCCESS;

    while (1)
    {
       signals = psa->wait(PSA_WAIT_ANY, PSA_BLOCK);
       if ((signals & SERVER_UNSPECIFED_VERSION_SIGNAL) == 0)
       {
          val->print(PRINT_ERROR,
                    "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
          return VAL_STATUS_ERROR;
       }

       if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
           continue;

       switch(msg.type)
       {
           case PSA_IPC_CONNECT:
                /* serve bulk connect cases to reach connect limit */
                count++;
                psa->reply(msg.handle, PSA_SUCCESS);
                break;
           case PSA_IPC_DISCONNECT:
                /* serve bulk disconnect cases */
                count--;
                psa->reply(msg.handle, PSA_SUCCESS);
       }
       if (count == 0)
       {
           /* Closed all open connection. Come out of loop */
           break;
       }
    }
    return status;
}

int32_t server_test_psa_block_behave(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             i = 0;

    /*
     * This is a sanity check - a successful handshaking between client and
     * server for requested connection represents check pass.
     */

    /* Debug print */
    val->err_check_set(TEST_CHECKPOINT_NUM(217), VAL_STATUS_SUCCESS);

    for (i = 0; i < CONNECT_NUM; i++)
    {
wait:
         /* PSA_BLOCK ored with 0xFF to check timeout[30:0]=RES is ignored by implementation */
         signals = psa->wait(PSA_WAIT_ANY, PSA_BLOCK | 0xff);

         /* When MODE is PSA_BLOCK, the psa_wait must return non-zero signal value */
         if ((signals & SERVER_UNSPECIFED_VERSION_SIGNAL) == 0)
         {
             val->print(PRINT_ERROR,
                     "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
             return VAL_STATUS_ERROR;
         }
         else
         {
             if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
             {
                 goto wait;
             }

             psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
         }
    }

    /* Debug print */
    val->err_check_set(TEST_CHECKPOINT_NUM(218), VAL_STATUS_SUCCESS);

    return VAL_STATUS_SUCCESS;
}

int32_t server_test_psa_poll_behave(void)
{
    psa_signal_t    signals_block = 0, signals_poll = 0;
    psa_msg_t       msg = {0};
    int             count = 0;

    while (1)
    {
        /* Debug print */
        val->err_check_set(TEST_CHECKPOINT_NUM(219), VAL_STATUS_SUCCESS);

        /* Wait for client request */
        signals_block = psa->wait(PSA_WAIT_ANY, PSA_BLOCK);

        /* Debug print */
        val->err_check_set(TEST_CHECKPOINT_NUM(220), VAL_STATUS_SUCCESS);

        /*
         * When MODE is PSA_POLL, the psa_wait will return immediately with
         * the current signal state. Expecting following call to return immediately
         * with signal_poll = signal_block.
         */
        signals_poll = psa->wait(PSA_WAIT_ANY, PSA_POLL);

        if (signals_poll != signals_block)
        {
            val->print(PRINT_ERROR, "\tSignal value mismatch\n", 0);
            val->print(PRINT_ERROR, "\tsignals_poll = 0x%x\n", signals_poll);
            val->print(PRINT_ERROR, "\tsignals_block = 0x%x\n", signals_block);
            return VAL_STATUS_ERROR;
        }
        else if ((signals_block & SERVER_UNSPECIFED_VERSION_SIGNAL) == 0)
        {
            val->print(PRINT_ERROR,
                    "\tpsa_wait returned with invalid signal_block = 0x%x\n", signals_block);
            return VAL_STATUS_ERROR;
        }
        else
        {
            if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
                continue;
            psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
            count++;
            signals_block = 0;
        }

        /* Come out of loop as we reached required connect limit*/
        if (count == CONNECT_NUM)
            break;
    }
    return VAL_STATUS_SUCCESS;
}
