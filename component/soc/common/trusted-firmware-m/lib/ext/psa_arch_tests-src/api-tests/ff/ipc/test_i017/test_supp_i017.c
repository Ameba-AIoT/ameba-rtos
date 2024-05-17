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

int32_t server_test_partition_calling_its_own_rot_service(void);

const server_test_t test_i017_server_tests_list[] = {
    NULL,
    server_test_partition_calling_its_own_rot_service,
    NULL,
};

int32_t server_test_partition_calling_its_own_rot_service(void)
{
    psa_msg_t         msg = {0};
    int32_t           status = VAL_STATUS_SUCCESS;
    psa_handle_t      handle = 0;

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

   /*
    * This test checks for the PROGRAMMER ERROR condition for the PSA API. API's respond to
    * PROGRAMMER ERROR could be either to return appropriate status code or panic the caller.
    * When a Secure Partition panics, the SPE cannot continue normal execution, as defined
    * in this specification. The behavior of the SPM following a Secure Partition panic is
    * IMPLEMENTATION DEFINED- Arm recommends that the SPM causes the system to restart in
    * this situation. Refer PSA-FF for more information on panic.
    * For the cases where, SPM cannot capable to reboot the system (just hangs or power down),
    * a watchdog timer set by val_test_init can reboot the system on timeout event. This will
    * tests continuity and able to jump to next tests. Therefore, each test who checks for
    * PROGRAMMER ERROR condition, expects system to get reset either by SPM or watchdog set by
    * the test harness function.
    *
    * If programmed timeout value isn't sufficient for your system, it can be reconfigured using
    * timeout entries available in target.cfg.
    *
    * To decide, a reboot happened as intended by test scenario or it happended
    * due to other reasons, test is setting a boot signature into non-volatile memory before and
    * after targeted test check. After a reboot, these boot signatures are being read by the
    * VAL APIs to decide test status.
    */

    /* Setting boot.state before test check */
    status = val->set_boot_flag(BOOT_EXPECTED_NS);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    /* Calling server partition RoT service using IPC and call should panic */
    handle = psa->connect(SERVER_RELAX_VERSION_SID, SERVER_RELAX_VERSION_VERSION);

    /* shouldn't have reached here */
    val->print(PRINT_ERROR, "\tConnection should failed but succeed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    status = val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }

    status = VAL_STATUS_SPM_FAILED;
    val->err_check_set(TEST_CHECKPOINT_NUM(204), status);

    (void)(handle);
    psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
    return status;
}
