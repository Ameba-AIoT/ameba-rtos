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

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

int32_t server_test_nspe_read_app_rot_variable(void);
int32_t server_test_nspe_write_app_rot_variable(void);

#define DATA_VALUE 0x5467

/* Application RoT data region */
volatile uint32_t g_test_i072 = DATA_VALUE;

#if STATELESS_ROT == 1

const server_test_t test_i072_server_tests_list[] = {
    NULL,
    server_test_nspe_read_app_rot_variable,
    server_test_nspe_write_app_rot_variable,
    NULL,
};

static int32_t send_secure_partition_address(void)
{
    int32_t           status = VAL_STATUS_SUCCESS;
    volatile uint32_t *addr = &g_test_i072;
    psa_msg_t         msg = {0};

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, -2);
        return status;
    }

    val->print(PRINT_DEBUG, "\tAPP-ROT: Passing 0x%x to NSPE\n", (int)&g_test_i072);

    /* Send Application RoT data address - global variable */
    psa->write(msg.handle, 0, (void *)&addr, sizeof(addr));
    psa->reply(msg.handle, PSA_SUCCESS);

    return VAL_STATUS_SUCCESS;
}

int32_t server_test_nspe_read_app_rot_variable(void)
{
    return send_secure_partition_address();
}

int32_t server_test_nspe_write_app_rot_variable(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;


    status = send_secure_partition_address();
    if (VAL_ERROR(status))
        return status;

    /* Reached here means there could be write succeed or ignored */
    if (g_test_i072 == DATA_VALUE)
        return VAL_STATUS_SUCCESS;

    val->print(PRINT_ERROR, "\tExpected write to fault but it didn't\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }
    return VAL_STATUS_SUCCESS;
}

#else

const server_test_t test_i072_server_tests_list[] = {
    NULL,
    server_test_nspe_read_app_rot_variable,
    server_test_nspe_write_app_rot_variable,
    NULL,
};

static int32_t send_secure_partition_address(void)
{
    int32_t           status = VAL_STATUS_SUCCESS;
    volatile uint32_t *addr = &g_test_i072;
    psa_msg_t         msg = {0};

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, -2);
        return status;
    }

    val->print(PRINT_DEBUG, "\tAPP-ROT: Passing 0x%x to NSPE\n", (int)&g_test_i072);

    /* Send Application RoT data address - global variable */
    psa->write(msg.handle, 0, (void *)&addr, sizeof(addr));
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    return VAL_STATUS_SUCCESS;
}

int32_t server_test_nspe_read_app_rot_variable(void)
{
    return send_secure_partition_address();
}

int32_t server_test_nspe_write_app_rot_variable(void)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_msg_t       msg = {0};

    status = send_secure_partition_address();
    if (VAL_ERROR(status))
        return status;

    /* Wait for write to get performed by client */
    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(204), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /* Connection request is just for handshake, reject connection anyways */
    psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);

    /* Reached here means there could be write succeed or ignored */
    if (g_test_i072 == DATA_VALUE)
        return VAL_STATUS_SUCCESS;

    val->print(PRINT_ERROR, "\tExpected write to fault but it didn't\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
        return VAL_STATUS_ERROR;
    }
    return VAL_STATUS_SUCCESS;
}

#endif