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

#if STATELESS_ROT == 1

int32_t server_test_psa_get_with_invalid_msg_pointer(void);

const server_test_t test_i047_server_tests_list[] = {
    NULL,
    server_test_psa_get_with_invalid_msg_pointer,
    NULL,
};

int32_t server_test_psa_get_with_invalid_msg_pointer(void)
{
   int32_t                 status = VAL_STATUS_SUCCESS;
   psa_signal_t            signals = 0;
   psa_msg_t               *invalid_msg = NULL;
   memory_desc_t           *memory_desc;

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

    signals = psa->wait(SERVER_UNSPECIFED_VERSION_SIGNAL, PSA_BLOCK);
    if ((signals & SERVER_UNSPECIFED_VERSION_SIGNAL) == 0)
    {
       val->print(PRINT_ERROR,
                "psa_wait returned with invalid signal value = 0x%x\n", signals);
       return VAL_STATUS_ERROR;
    }

    /*
     * Selection of invalid msg pointer:
     *
     *  if (ISOLATION_LEVEL > 1)
     *       msg_pointer = driver_mmio_region_base;
     *  else
     *       msg_pointer = NULL;
     */

    if (PLATFORM_PSA_ISOLATION_LEVEL > LEVEL1)
    {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                      MEMORY_DRIVER_PARTITION_MMIO, 0),
                                      (uint8_t **)&memory_desc,
                                      (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
        {
            return status;
        }

        invalid_msg = (psa_msg_t *) memory_desc->start;
    }

    /* Setting boot.state before test check */
    if (val->set_boot_flag(BOOT_EXPECTED_NS))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_get with invalid msg pointer, call should panic */
    psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, invalid_msg);

    /* shouldn't have reached here */
    val->print(PRINT_ERROR,
             "\tpsa_get with invalid msg pointer should failed but succeed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
    }

    return VAL_STATUS_SPM_FAILED;
}

#else

int32_t server_test_psa_get_with_invalid_msg_pointer(void);

const server_test_t test_i047_server_tests_list[] = {
    NULL,
    server_test_psa_get_with_invalid_msg_pointer,
    NULL,
};

int32_t server_test_psa_get_with_invalid_msg_pointer(void)
{
   int32_t                 status = VAL_STATUS_SUCCESS;
   psa_signal_t            signals = 0;
   psa_msg_t               *invalid_msg = NULL;
   memory_desc_t           *memory_desc;

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

    signals = psa->wait(SERVER_UNSPECIFED_VERSION_SIGNAL, PSA_BLOCK);
    if ((signals & SERVER_UNSPECIFED_VERSION_SIGNAL) == 0)
    {
       val->print(PRINT_ERROR,
                "psa_wait returned with invalid signal value = 0x%x\n", signals);
       return VAL_STATUS_ERROR;
    }

    /*
     * Selection of invalid msg pointer:
     *
     *  if (ISOLATION_LEVEL > 1)
     *       msg_pointer = driver_mmio_region_base;
     *  else
     *       msg_pointer = NULL;
     */

    if (PLATFORM_PSA_ISOLATION_LEVEL > LEVEL1)
    {
        status = val->target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY,
                                      MEMORY_DRIVER_PARTITION_MMIO, 0),
                                      (uint8_t **)&memory_desc,
                                      (uint32_t *)sizeof(memory_desc_t));
        if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
        {
            return status;
        }

        invalid_msg = (psa_msg_t *) memory_desc->start;
    }

    /* Setting boot.state before test check */
    if (val->set_boot_flag(BOOT_EXPECTED_NS))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag before check\n", 0);
        return VAL_STATUS_ERROR;
    }

    /* psa_get with invalid msg pointer, call should panic */
    psa->get(SERVER_UNSPECIFED_VERSION_SIGNAL, invalid_msg);

    /* shouldn't have reached here */
    val->print(PRINT_ERROR,
             "\tpsa_get with invalid msg pointer should failed but succeed\n", 0);

    /* Resetting boot.state to catch unwanted reboot */
    if (val->set_boot_flag(BOOT_EXPECTED_BUT_FAILED))
    {
        val->print(PRINT_ERROR, "\tFailed to set boot flag after check\n", 0);
    }

    /* Reject the connection */
    psa->reply(invalid_msg->handle, PSA_ERROR_CONNECTION_REFUSED);

    return VAL_STATUS_SPM_FAILED;
}

#endif