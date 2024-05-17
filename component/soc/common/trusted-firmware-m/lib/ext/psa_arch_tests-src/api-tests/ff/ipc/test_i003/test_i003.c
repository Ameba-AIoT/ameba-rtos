/** @file
 * Copyright (c) 2018-2021, Arm Limited or its affiliates. All rights reserved.
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

#include "test_i003.h"

#if STATELESS_ROT == 1

const client_test_t test_i003_client_tests_list[] = {
    NULL,
    client_test_zero_length_invec,
    client_test_zero_length_outvec,
    client_test_call_read_and_skip,
    client_test_call_and_write,
    client_test_overlapping_vectors,
    NULL,
};

int32_t client_test_zero_length_invec(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;

   int                  data[4] = {0x11, 0x22};

   val->print(PRINT_TEST, "[Check 1] Test zero length invec\n", 0);

   /* Invec 0 as zero length vector
    * Invec 1 as NULL
    * Invec 2 as valid msg pointer which is not zero length memory pointer
   */
   psa_invec data1[3] = { {&data[0], 0},
                        {NULL, 0},
                        {&data[1], sizeof(data[1])} };

   /* Outvec 0 as valid msg pointer which is not zero length memory pointer
    * Outvec 1 to 3 are NULL
    */
   psa_outvec resp[1] = { {&data[2], sizeof(data[2])} };

   status = psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, data1, 3, resp, 1);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   else if (data[2] != data[1])
   {
       val->print(PRINT_ERROR, "\tExpected data=%x\n", data[1]);
       val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[2]);
       status = VAL_STATUS_WRITE_FAILED;
   }
   else if (resp[0].len != sizeof(data[1]))
   {
       val->print(PRINT_ERROR, "\tExpected size= %x\n", sizeof(data[1]));
       val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[0].len);
       status = VAL_STATUS_WRITE_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   return status;
}

int32_t client_test_zero_length_outvec(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;

   int                  data[4] = {0x11};

   val->print(PRINT_TEST, "[Check 2] Test zero length outvec\n", 0);

   /* Test invec 1 to 3 are NULL
    * Invec 1 should be ignored since in_len is 1
    */
   psa_invec data1[2] = { {&data[0], sizeof(data[0])},
                          {&data[1], sizeof(data[1])} };

   /* Test outvec 0 as zero length
    * Test outvec 1 as NULL
    * Test outvec 2 as valid msg pointer which is not zero length memory pointer
   */
   psa_outvec resp[3] = { {&data[0], 0},
                          {NULL, 0},
                          {&data[2], sizeof(data[2])} };

   status = psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, data1, 1, resp, 3);


   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   /* Compare the outvec with expected data */
   else if (data[2] != data[0])
   {
       val->print(PRINT_ERROR, "\tExpected data=%x\n", data[0]);
       val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[2]);
       status = VAL_STATUS_WRITE_FAILED;
   }
   /* No. of bytes written by psa_write should update the psa_outvec.len param */
   else if (resp[2].len != sizeof(data[2]))
   {
       val->print(PRINT_ERROR, "\tExpected size=%x\n", sizeof(data[2]));
       val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[2].len);
       status = VAL_STATUS_WRITE_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   return status;
}

int32_t client_test_call_read_and_skip(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   int                  data1[2] = {0xaa, 0xbb};
   uint64_t             data2 = 0x1122334455667788;
   uint64_t             data3 = 0x1020304050607080;


   val->print(PRINT_TEST, "[Check 3] Test psa_call, psa_read and psa_skip\n", 0);

   /* Server test func checks the following:
    * All iovec as input, out_len=0, inbound read, inbound skip,
    * outbound read, outbound skip
    */
   psa_invec data[4] = { {&data1[0], sizeof(data1[0])},
                         {&data1[1], sizeof(data1[1])},
                         {&data2, sizeof(data2)},
                         {&data3, sizeof(data3)} };

   status = psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, data, 4, NULL, 0);


   if (status < 0)
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
       status = VAL_STATUS_CALL_FAILED;
   }

   return status;
}

int32_t client_test_call_and_write(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   int                  data[4] = {0}, expected_data[4] = {0xaa, 0xbb, 0xcc, 0xeedd}, i;
   size_t               expected_size[] = {sizeof(expected_data[0]),
                                           sizeof(expected_data[1]),
                                           sizeof(expected_data[2]),
                                           2};


   val->print(PRINT_TEST, "[Check 4] Test psa_call and psa_write\n", 0);

   /* Check all iovec as output */
   /* Check for in_len=0 */
   /* check for iovec write using psa_write */
   /* check for write concatenation */
   psa_outvec resp[4] = { {&data[0], sizeof(data[0])},
                          {&data[1], sizeof(data[1])},
                          {&data[2], sizeof(data[2])},
                          {&data[3], sizeof(data[3])} };

   status = psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, NULL, 0, resp, 4);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }

   for (i = 0; i < 4 ; i++)
   {
        if (data[i] != expected_data[i])
        {
            val->print(PRINT_ERROR, "\tIteration= %x\n", i);
            val->print(PRINT_ERROR, "\tExpected data=%x\n", expected_data[i]);
            val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[i]);
            status = VAL_STATUS_WRITE_FAILED;
        }
        /* No. of bytes written by psa_write should update the psa_outvec.len param */
        else if (resp[i].len != expected_size[i])
        {
            val->print(PRINT_ERROR, "\tIteration= %x\n", i);
            val->print(PRINT_ERROR, "\tExpected size=%x\n", expected_size[i]);
            val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[i].len);
            status = VAL_STATUS_WRITE_FAILED;
        }
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   return status;
}

int32_t client_test_overlapping_vectors(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;

   uint8_t            data = 0x11, expected_data[] = {0x22, 0x33};

   val->print(PRINT_TEST, "[Check 6] Test overlapping vectors\n", 0);

#if STATELESS_ROT != 1
   psa_handle_t       handle = 0;
   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }
#endif

   /* Rule- When client provides an input and output vectors which are referencing
    * to same memory location, psa_read after psa_write to that memory location
    * can return original or modified value.
    *
    * Rule- When client provides an input and output vectors which are referencing to same
    * memory location, a psa_write(s) to both memory vectors can return either the 1st
    * or the 2nd value written.
    *
    * Input [0] and output [0,1] vectors have been used to verify these rule.
    * All of these vectors are pointing to same memory location.
    */

   psa_invec invec[1] = { {&data, sizeof(data)} } ;
   psa_outvec outvec[2] = { {&data, sizeof(data)},
                            {&data, sizeof(data)} };

   status = psa->call(SERVER_UNSPECIFED_VERSION_HANDLE, PSA_IPC_CALL, invec, 1, outvec, 2);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   /* data should contain either value written by 1st psa_write or 2nd */
   else if ((data != expected_data[0]) && (data != expected_data[1]))
   {
       val->print(PRINT_ERROR, "\tInvalid data received=%x\n", data);
       status = VAL_STATUS_CALL_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   return status;
}

#else

const client_test_t test_i003_client_tests_list[] = {
    NULL,
    client_test_zero_length_invec,
    client_test_zero_length_outvec,
    client_test_call_read_and_skip,
    client_test_call_and_write,
    client_test_psa_set_rhandle,
    client_test_overlapping_vectors,
    NULL,
};

int32_t client_test_zero_length_invec(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   psa_handle_t         handle = 0;
   int                  data[4] = {0x11, 0x22};

   val->print(PRINT_TEST, "[Check 1] Test zero length invec\n", 0);

   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

   /* Invec 0 as zero length vector
    * Invec 1 as NULL
    * Invec 2 as valid msg pointer which is not zero length memory pointer
   */
   psa_invec data1[3] = {{&data[0], 0},
                        {NULL, 0},
                        {&data[1], sizeof(data[1])}};

   /* Outvec 0 as valid msg pointer which is not zero length memory pointer
    * Outvec 1 to 3 are NULL
    */
   psa_outvec resp[1] = {{&data[2], sizeof(data[2])}};

   status = psa->call(handle, PSA_IPC_CALL, data1, 3, resp, 1);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   else if (data[2] != data[1])
   {
       val->print(PRINT_ERROR, "\tExpected data=%x\n", data[1]);
       val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[2]);
       status = VAL_STATUS_WRITE_FAILED;
   }
   else if (resp[0].len != sizeof(data[1]))
   {
       val->print(PRINT_ERROR, "\tExpected size= %x\n", sizeof(data[1]));
       val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[0].len);
       status = VAL_STATUS_WRITE_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }
   val->ipc_close(handle);

   return status;
}

int32_t client_test_zero_length_outvec(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   psa_handle_t         handle = 0;
   int                  data[4] = {0x11};

   val->print(PRINT_TEST, "[Check 2] Test zero length outvec\n", 0);

   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

   /* Test invec 1 to 3 are NULL
    * Invec 1 should be ignored since in_len is 1
    */
   psa_invec data1[2] = {{&data[0], sizeof(data[0])},
                         {&data[1], sizeof(data[1])}};

   /* Test outvec 0 as zero length
    * Test outvec 1 as NULL
    * Test outvec 2 as valid msg pointer which is not zero length memory pointer
   */
   psa_outvec resp[3] = {{&data[0], 0},
                         {NULL, 0},
                         {&data[2], sizeof(data[2])}};

   status = psa->call(handle, PSA_IPC_CALL, data1, 1, resp, 3);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   /* Compare the outvec with expected data */
   else if (data[2] != data[0])
   {
       val->print(PRINT_ERROR, "\tExpected data=%x\n", data[0]);
       val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[2]);
       status = VAL_STATUS_WRITE_FAILED;
   }
   /* No. of bytes written by psa_write should update the psa_outvec.len param */
   else if (resp[2].len != sizeof(data[2]))
   {
       val->print(PRINT_ERROR, "\tExpected size=%x\n", sizeof(data[2]));
       val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[2].len);
       status = VAL_STATUS_WRITE_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   val->ipc_close(handle);

   return status;
}

int32_t client_test_call_read_and_skip(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   int                  data1[2] = {0xaa, 0xbb};
   uint64_t             data2 = 0x1122334455667788;
   uint64_t             data3 = 0x1020304050607080;
   psa_handle_t         handle = 0;

   val->print(PRINT_TEST, "[Check 3] Test psa_call, psa_read and psa_skip\n", 0);

   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

   /* Server test func checks the following:
    * All iovec as input, out_len=0, inbound read, inbound skip,
    * outbound read, outbound skip
    */
   psa_invec data[4] = {{&data1[0], sizeof(data1[0])},
                        {&data1[1], sizeof(data1[1])},
                        {&data2, sizeof(data2)},
                        {&data3, sizeof(data3)}};

   status = psa->call(handle, PSA_IPC_CALL, data, 4, NULL, 0);

   if (status < 0)
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n",status);
       status = VAL_STATUS_CALL_FAILED;
   }

   val->ipc_close(handle);

   return status;
}

int32_t client_test_call_and_write(caller_security_t caller __UNUSED)
{
   int32_t              status = VAL_STATUS_SUCCESS;
   int                  data[4] = {0}, expected_data[4] = {0xaa, 0xbb, 0xcc, 0xeedd}, i;
   size_t               expected_size[] = {sizeof(expected_data[0]),
                                           sizeof(expected_data[1]),
                                           sizeof(expected_data[2]),
                                           2};
   psa_handle_t         handle = 0;

   val->print(PRINT_TEST, "[Check 4] Test psa_call and psa_write\n", 0);

   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

   /* Check all iovec as output */
   /* Check for in_len=0 */
   /* check for iovec write using psa_write */
   /* check for write concatenation */
   psa_outvec resp[4] = {{&data[0], sizeof(data[0])},
                         {&data[1], sizeof(data[1])},
                         {&data[2], sizeof(data[2])},
                         {&data[3], sizeof(data[3])}};

   status = psa->call(handle, PSA_IPC_CALL, NULL, 0, resp, 4);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }

   for (i = 0; i < 4 ; i++)
   {
        if (data[i] != expected_data[i])
        {
            val->print(PRINT_ERROR, "\tIteration= %x\n", i);
            val->print(PRINT_ERROR, "\tExpected data=%x\n", expected_data[i]);
            val->print(PRINT_ERROR, "\tBut actual data=%x\n", data[i]);
            status = VAL_STATUS_WRITE_FAILED;
        }
        /* No. of bytes written by psa_write should update the psa_outvec.len param */
        else if (resp[i].len != expected_size[i])
        {
            val->print(PRINT_ERROR, "\tIteration= %x\n", i);
            val->print(PRINT_ERROR, "\tExpected size=%x\n", expected_size[i]);
            val->print(PRINT_ERROR, "\tBut actual size=%x\n", resp[i].len);
            status = VAL_STATUS_WRITE_FAILED;
        }
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   val->ipc_close(handle);

   return status;
}

int32_t client_test_psa_set_rhandle(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;
   psa_handle_t       handle = 0;
   int                i = 0;

   val->print(PRINT_TEST, "[Check 5] Test psa_set_rhandle API\n", 0);

   /* rhandle value check when PSA_IPC_CONNECT */
   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

    for (i = 0; i < 3; i++)
    {
        /* i=0: rhandle value check when PSA_IPC_CALL
         * i=1: rhandle value check after calling psa_set_rhandle()
         * i:2: rhandle value check after resetting rhandle to other value
         */
        status = psa->call(handle, PSA_IPC_CALL, NULL, 0, NULL, 0);
        if (status < 0)
        {
            val->print(PRINT_ERROR, "psa_call failed. status=%x\n", status);
            status = VAL_STATUS_CALL_FAILED;
            break;
        }
    }

   /*rhandle value check when PSA_IPC_DISCONNECT */
   val->ipc_close(handle);
   return status;
}

int32_t client_test_overlapping_vectors(caller_security_t caller __UNUSED)
{
   int32_t            status = VAL_STATUS_SUCCESS;

   uint8_t            data = 0x11, expected_data[] = {0x22, 0x33};

   val->print(PRINT_TEST, "[Check 6] Test overlapping vectors\n", 0);

   psa_handle_t       handle = 0;
   if (val->ipc_connect(SERVER_UNSPECIFED_VERSION_SID,
                        SERVER_UNSPECIFED_VERSION_VERSION,
                        &handle))
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }

   /* Rule- When client provides an input and output vectors which are referencing
    * to same memory location, psa_read after psa_write to that memory location
    * can return original or modified value.
    *
    * Rule- When client provides an input and output vectors which are referencing to same
    * memory location, a psa_write(s) to both memory vectors can return either the 1st
    * or the 2nd value written.
    *
    * Input [0] and output [0,1] vectors have been used to verify these rule.
    * All of these vectors are pointing to same memory location.
    */

   psa_invec invec[1] = {{&data, sizeof(data)}};
   psa_outvec outvec[2] = {{&data, sizeof(data)},
                           {&data, sizeof(data)}};

   status = psa->call(handle, PSA_IPC_CALL, invec, 1, outvec, 2);

   if (status < 0)
   {
       status = VAL_STATUS_CALL_FAILED;
   }
   /* data should contain either value written by 1st psa_write or 2nd */
   else if ((data != expected_data[0]) && (data != expected_data[1]))
   {
       val->print(PRINT_ERROR, "\tInvalid data received=%x\n", data);
       status = VAL_STATUS_CALL_FAILED;
   }

   if (VAL_ERROR(status))
   {
       val->print(PRINT_ERROR, "\tpsa_call failed. status=%x\n", status);
   }

   val->ipc_close(handle);

   return status;
}

#endif
