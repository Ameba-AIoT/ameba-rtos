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

int32_t server_test_secure_access_only_connection(void);

const server_test_t test_i008_server_tests_list[] = {
    NULL,
    server_test_secure_access_only_connection,
    NULL,
};

int32_t server_test_secure_access_only_connection(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    val->err_check_set(TEST_CHECKPOINT_NUM(201), status);
    status = val->process_connect_request(SERVER_SECURE_CONNECT_ONLY_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    if (msg.client_id < 0)
    {
        /*
		 * Control shouldn't come here. NS client should get panic.
		 * Resetting boot.state to catch unwanted reboot.
		 * */
        if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
        {
           val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
        }

        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return VAL_STATUS_SPM_FAILED;
    }
    else
    {
        psa->reply(msg.handle, PSA_SUCCESS);
    }

    status = val->process_disconnect_request(SERVER_SECURE_CONNECT_ONLY_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);
    return status;
}
