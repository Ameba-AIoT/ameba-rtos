/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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

int32_t server_test_psa_wait_signal_mask(void);

const server_test_t test_i063_server_tests_list[] = {
    NULL,
    server_test_psa_wait_signal_mask,
    NULL,
};

int32_t server_test_psa_wait_signal_mask(void)
{
    psa_signal_t    signals = 0;
    psa_msg_t       msg = {0};
    int             loop_cnt = 2;
    psa_signal_t    signal_mask = (SERVER_UNSPECIFED_VERSION_SIGNAL | SERVER_RELAX_VERSION_SIGNAL);

    /* Debug print */
    val->err_check_set(TEST_CHECKPOINT_NUM(211), VAL_STATUS_SUCCESS);

    /*
     * Notify client partition to make SERVER_SECURE_CONNECT_ONLY_SID connection request.
     * This connection request act as irritator to psa->wait(signal_mask) call and it is used
     * to cover the rule - Signals that are not in signal_mask should be ignored by psa_wait.
     * This means, during the following while loop, returned signal value should not be
     * SERVER_SECURE_CONNECT_ONLY_SIGNAL as this signal is not part of signal_mask.
     */
    psa->notify(CLIENT_PARTITION);

    while (loop_cnt != 0)
    {
         signals = psa->wait(signal_mask, PSA_BLOCK);

         /*
          * Rule - Returned signals value must be subset signals indicated in the signal_mask.
          * This mean signal value should be either SERVER_UNSPECIFED_VERSION_SIGNAL
          * or SERVER_RELAX_VERSION_SIGNAL.
          */
         if (((signals & signal_mask) == 0) &&
             ((signals | signal_mask) != signal_mask))
         {
             val->print(PRINT_ERROR,
                     "psa_wait-1 returned with invalid signal value = 0x%x\n", signals);
             return VAL_STATUS_ERROR;
         }
         else if (signals & SERVER_UNSPECIFED_VERSION_SIGNAL)
         {
             if (psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
                 continue;

             loop_cnt--;
             psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
         }
         else if (signals & SERVER_RELAX_VERSION_SIGNAL)
         {
             if (psa->get(SERVER_RELAX_VERSION_SIGNAL, &msg) != PSA_SUCCESS)
                 continue;

             loop_cnt--;
             psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
         }
    }

    /* Debug print */
    val->err_check_set(TEST_CHECKPOINT_NUM(212), VAL_STATUS_SUCCESS);

wait:
    /*
     * At the end, completes the starved connection
     * request of SERVER_SECURE_CONNECT_ONLY_SID.
     */
    signals = psa->wait(SERVER_SECURE_CONNECT_ONLY_SIGNAL, PSA_BLOCK);
    if ((signals & SERVER_SECURE_CONNECT_ONLY_SIGNAL) == 0)
    {
       val->print(PRINT_ERROR,
                "psa_wait-2 returned with invalid signal value = 0x%x\n", signals);
       return VAL_STATUS_ERROR;
    }

    if (psa->get(SERVER_SECURE_CONNECT_ONLY_SIGNAL, &msg) != PSA_SUCCESS)
        goto wait;

    psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
    return VAL_STATUS_SUCCESS;
}
