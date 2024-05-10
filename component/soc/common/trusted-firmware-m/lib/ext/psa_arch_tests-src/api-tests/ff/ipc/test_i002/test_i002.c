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

#ifdef NONSECURE_TEST_BUILD
#include "val_interfaces.h"
#include "val_target.h"
#else
#include "val_client_defs.h"
#include "val_service_defs.h"
#endif

#include "test_i002.h"

const client_test_t test_i002_client_tests_list[] = {
    NULL,
    client_test_connection_busy_and_reject,
    client_test_accept_and_close_connect,
    client_test_connect_with_allowed_version_policy,
    client_test_psa_call_with_allowed_status_code,
    client_test_psa_call_with_allowed_type_values,
    client_test_identity,
    client_test_spm_concurrent_connect_limit,
    client_test_psa_block_behave,
    client_test_psa_poll_behave,
    NULL,
};

int32_t client_test_connection_busy_and_reject(caller_security_t caller __UNUSED)
{
    int32_t         status = VAL_STATUS_SUCCESS;
    psa_handle_t    handle = 0;

    val->print(PRINT_TEST, "[Check 1] Test busy and reject connect type\n", 0);

    handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);

    /*
     * The RoT Service can't make connection at this moment. It sends
     * PSA_ERROR_CONNECTION_BUSY in psa_reply.
     */
    if (handle != PSA_ERROR_CONNECTION_BUSY)
    {
        val->print(PRINT_ERROR,
                   "Expected handle=PSA_ERROR_CONNECTION_BUSY but handle=0x%x\n", handle);
        return VAL_STATUS_INVALID_HANDLE;
    }

    handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);

    /* The RoT Service rejected the client because of an application-specific case
     * Expect PSA_ERROR_CONNECTION_REFUSED as return
     */
    if (handle != PSA_ERROR_CONNECTION_REFUSED)
    {
        val->print(PRINT_ERROR,
                   "Expected handle=PSA_ERROR_CONNECTION_REFUSED but handle=0x%x\n", handle);
        status = VAL_STATUS_INVALID_HANDLE;
    }

   return status;
}

int32_t client_test_accept_and_close_connect(caller_security_t caller __UNUSED)
{
   psa_handle_t     handle = 0;

   val->print(PRINT_TEST, "[Check 2] Accept and close connection\n", 0);

   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
   /* RoT service accepts the connection. Expecting positive handle */
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       return VAL_STATUS_INVALID_HANDLE;
   }

   /* psa_close with PSA_NULL_HANDLE. This shoudn't have any effect */
   psa->close(PSA_NULL_HANDLE);

   /* Close the connection */
   psa->close(handle);
   return VAL_STATUS_SUCCESS;
}

int32_t client_test_connect_with_allowed_version_policy(caller_security_t caller __UNUSED)
{
   psa_handle_t     handle = 0;
   uint32_t         i = 0;
   uint32_t         sid[] = {SERVER_UNSPECIFED_VERSION_SID,
                             SERVER_STRICT_VERSION_SID,
                             SERVER_RELAX_VERSION_SID,
                             SERVER_RELAX_VERSION_SID};
   uint32_t         version[] = {SERVER_UNSPECIFED_VERSION_VERSION,
                                 SERVER_STRICT_VERSION_VERSION,
                                 SERVER_RELAX_VERSION_VERSION - 1,
                                 SERVER_RELAX_VERSION_VERSION};

   val->print(PRINT_TEST, "[Check 3] Test psa_connect with allowed version policy\n", 0);

   /* Connect RoT service with following version numbers and expect positive handle for
    * each connection:
    * Case 1. Version policy is not mentioned and requested version is 1 (default minimum version)
    * Case 2. Version policy is STRICT and requested version equals minimum version
    * Case 3. Version policy is relaxed and requested version is smaller than the minimum version
    * Case 4. Version policy is relaxed and requested version is euqal to the minimum version
   */

   for (i = 0; i < (sizeof(sid)/sizeof(sid[0])); i++)
   {
        handle = psa->connect(sid[i], version[i]);
        if (!PSA_HANDLE_IS_VALID(handle))
        {
            val->print(PRINT_ERROR,
                        "\tpsa_connect failed for version policy. Iteration No=%d\n", i);
            return VAL_STATUS_INVALID_HANDLE;
        }

        psa->close(handle);
   }

   return VAL_STATUS_SUCCESS;
}

int32_t psa_call_with_null_msg(int32_t expected_status)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;
   psa_status_t       status_of_call;

   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       return VAL_STATUS_INVALID_HANDLE;
   }

   status_of_call =  psa->call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);

   /* Compare status code returned with expected status code */
   if (status_of_call != expected_status)
   {
       status = VAL_STATUS_CALL_FAILED;
   }

   psa->close(handle);
   return status;
}

int32_t client_test_psa_call_with_allowed_status_code(caller_security_t caller __UNUSED)
{
   int32_t         status = VAL_STATUS_SUCCESS;
   psa_status_t    expected_status_code[] = {PSA_SUCCESS, 1, 2, INT32_MAX, -1, -2, INT32_MIN+128};
   uint32_t        i = 0;

   val->print(PRINT_TEST, "[Check 4] Test psa_call with allowed status code\n", 0);

   for (i = 0; i < (sizeof(expected_status_code)/sizeof(expected_status_code[0])); i++)
   {
        /* Compare the status code sent by RoT service with expected code */
        status = psa_call_with_null_msg(expected_status_code[i]);
        if (VAL_ERROR(status))
        {
            val->print(PRINT_ERROR,
                       "\tpsa_call failed for status code=0x%x\n", expected_status_code[i]);
            return status;
        }
   }
   return status;
}

int32_t client_test_psa_call_with_allowed_type_values(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;
   int16_t            type[] = {PSA_IPC_CALL, 1, 2, INT16_MAX};
   uint32_t           i = 0;

   val->print(PRINT_TEST, "[Check 5] Test psa_call with different type values\n", 0);
   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       return VAL_STATUS_INVALID_HANDLE;
   }

   for (i = 0; i < (sizeof(type)/sizeof(type[0])); i++)
   {
       /* Send type = type[i] */
       if (psa->call(handle, type[i], NULL, 0, NULL, 0) != PSA_SUCCESS)
       {
           val->print(PRINT_ERROR, "\tCheck failed for type=%d\n", type[i]);
           status = VAL_STATUS_CALL_FAILED;
           break;
       }
   }

   psa->close(handle);
   return status;
}

int32_t client_test_identity(caller_security_t caller)
{
   int32_t         status = VAL_STATUS_SUCCESS;
   psa_handle_t    handle = 0;
   psa_status_t    status_of_call;
   int             id_at_connect = 0, id_at_call = 0;

   val->print(PRINT_TEST, "[Check 6] Test client_id\n", 0);

   handle = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
   if (!PSA_HANDLE_IS_VALID(handle))
   {
       return(VAL_STATUS_INVALID_HANDLE);
   }

   psa_outvec resp[2] = {{&id_at_connect, sizeof(id_at_call)},
                         {&id_at_call, sizeof(id_at_call)}};

   status_of_call =  psa->call(handle, PSA_IPC_CALL, NULL, 0, resp, 2);


   if (status_of_call != PSA_SUCCESS)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   /* For NSPE access, identity should be < 0 */
   else if ((caller == CALLER_NONSECURE) && ((id_at_connect != id_at_call)
            || (id_at_connect >=0) || (id_at_call >=0)))
   {
       status = VAL_STATUS_WRONG_IDENTITY;
   }
   /* For SPE access, identity should be > 0 */
   else if ((caller == CALLER_SECURE) && ((id_at_connect != id_at_call)
            || (id_at_connect <=0) || (id_at_call <=0)))
   {
       status = VAL_STATUS_WRONG_IDENTITY;
   }
   psa->close(handle);

   return status;
}

int32_t client_test_spm_concurrent_connect_limit(caller_security_t caller __UNUSED)
{
   int32_t         status = VAL_STATUS_SUCCESS;
   psa_handle_t    handle[CONNECT_LIMIT] = {0};
   int             i= 0, signture = 0;

   val->print(PRINT_TEST, "[Check 7] Test connect limit\n", 0);

   if (caller == CALLER_SECURE)
   {
	   status = val->set_boot_flag(BOOT_EXPECTED_S);
	   if (VAL_ERROR(status))
	   {
		   return VAL_STATUS_ERROR;
	   }
   }

   /* Execute psa_connect in a loop until it returns
    * PSA_ERROR_CONNECTION_REFUSED OR PSA_ERROR_CONNECTION_BUSY
    */
   while (i < CONNECT_LIMIT)
   {
       handle[i] = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
       /* Compare handle value */
       if ((handle[i] == PSA_ERROR_CONNECTION_REFUSED) || (handle[i] == PSA_ERROR_CONNECTION_BUSY))
       {
           signture = 1;
           break;
       }
       i++;
   }

   if (signture != 1)
   {
       /* Didn't recieve expected return handle value */
       status = VAL_STATUS_SPM_FAILED;
       i--;
   }

   while (i >= 0)
   {
       if (handle[i] > 0)
       {
           /* Close all open connection to RoT service */
           psa->close(handle[i]);
       }
       i--;
   }
   return status;
}

int32_t client_test_psa_wait(void)
{
   psa_handle_t    handle[CONNECT_NUM] = {0};
   int             i = 0;

   for (i = 0; i < CONNECT_NUM; i++)
   {
       handle[i] = psa->connect(SERVER_UNSPECIFED_VERSION_SID, SERVER_UNSPECIFED_VERSION_VERSION);
       if (handle[i] != PSA_ERROR_CONNECTION_REFUSED)
       {
           return VAL_STATUS_INVALID_HANDLE;
       }
   }

   return VAL_STATUS_SUCCESS;
}

int32_t client_test_psa_block_behave(caller_security_t caller __UNUSED)
{
   val->print(PRINT_TEST, "[Check 8] Test PSA_BLOCK\n", 0);
   return (client_test_psa_wait());
}

int32_t client_test_psa_poll_behave(caller_security_t caller __UNUSED)
{
   val->print(PRINT_TEST, "[Check 9] Test PSA_POLL\n", 0);
   return (client_test_psa_wait());
}
