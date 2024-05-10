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

/* Note- This file contains the functions and variables definition which are common to
   all partitions defined by test suite. These functions and variables are declared with static
   keyword because some fully isolated system may not allow to share code and data segment
   between partitions and static will help each partition to have its own copy of code and data.
   Moreover it can prevents symbol names conflict if these functions are separately compiled and
   linked with each of partitions in fully isolated environment.
*/

#ifndef _VAL_COMMON_SP_APIS_H_
#define _VAL_COMMON_SP_APIS_H_

#include "val.h"
#include "val_target.c"
#include "val_service_defs.h"

__UNUSED STATIC_DECLARE val_status_t val_print
                        (print_verbosity_t verbosity, char *string, int32_t data);
__UNUSED STATIC_DECLARE val_status_t val_ipc_connect
                        (uint32_t sid, uint32_t version, psa_handle_t *handle );
__UNUSED STATIC_DECLARE val_status_t val_ipc_call(psa_handle_t handle,
                                                  int32_t type,
                                                  const psa_invec *in_vec,
                                                  size_t in_len,
                                                  psa_outvec *out_vec,
                                                  size_t out_len);
__UNUSED STATIC_DECLARE void val_ipc_close
                        (psa_handle_t handle);
__UNUSED STATIC_DECLARE val_status_t val_process_connect_request(psa_signal_t sig, psa_msg_t *msg);
__UNUSED STATIC_DECLARE val_status_t val_process_call_request(psa_signal_t sig, psa_msg_t *msg);
__UNUSED STATIC_DECLARE val_status_t val_process_disconnect_request
                        (psa_signal_t sig, psa_msg_t *msg);
__UNUSED STATIC_DECLARE val_status_t val_execute_secure_tests
                        (test_info_t test_info, client_test_t *tests_list);
__UNUSED STATIC_DECLARE val_status_t val_execute_secure_test_func
                        (psa_handle_t *handle, test_info_t test_info, uint32_t sid);
__UNUSED STATIC_DECLARE val_status_t val_get_secure_test_result(psa_handle_t *handle);
__UNUSED STATIC_DECLARE val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status);
__UNUSED STATIC_DECLARE val_status_t val_nvmem_write(uint32_t offset, void *buffer, int size);
__UNUSED STATIC_DECLARE val_status_t val_set_boot_flag(boot_state_t state);
__UNUSED STATIC_DECLARE val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data);

__UNUSED static val_api_t val_api = {
    .print                     = val_print,
    .err_check_set             = val_err_check_set,
    .execute_secure_test_func  = val_execute_secure_test_func,
    .get_secure_test_result    = val_get_secure_test_result,
    .ipc_connect               = val_ipc_connect,
    .ipc_call                  = val_ipc_call,
    .ipc_close                 = val_ipc_close,
    .set_boot_flag             = val_set_boot_flag,
	.set_test_data             = val_set_test_data,
    .target_get_config         = val_target_get_config,
    .process_connect_request   = val_process_connect_request,
    .process_call_request      = val_process_call_request,
    .process_disconnect_request= val_process_disconnect_request,
};

__UNUSED static psa_api_t psa_api = {
    .framework_version     = psa_framework_version,
    .version               = psa_version,
    .connect               = psa_connect,
    .call                  = psa_call,
    .close                 = psa_close,
    .wait                  = psa_wait,
    .set_rhandle           = psa_set_rhandle,
    .get                   = psa_get,
    .read                  = psa_read,
    .skip                  = psa_skip,
    .write                 = psa_write,
    .reply                 = psa_reply,
    .notify                = psa_notify,
    .clear                 = psa_clear,
    .eoi                   = psa_eoi,
    .rot_lifecycle_state   = psa_rot_lifecycle_state,
    .panic                 = psa_panic,
};

/**
    @brief    - Print module. This is client interface API of secure partition
                val_print_sf API for spe world
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_print(print_verbosity_t verbosity, char *string, int32_t data)
{
    int             string_len = 0;
    char            *p = string;

    psa_status_t    status_of_call = PSA_SUCCESS;
    val_status_t    status = VAL_STATUS_SUCCESS;
    uart_fn_type_t  uart_fn = UART_PRINT;

    if (verbosity < VERBOSE)
    {
       return VAL_STATUS_SUCCESS;
    }

    while (*p != '\0')
    {
        string_len++;
        p++;
    }
    psa_invec data1[3] = {{&uart_fn, sizeof(uart_fn)}, {string, string_len+1}, {&data, sizeof(data)}};
#if STATELESS_ROT == 1

    status_of_call = psa_call(DRIVER_UART_HANDLE, 0, data1, 3, NULL, 0);
    if (status_of_call != PSA_SUCCESS)
    {
    	status = VAL_STATUS_CALL_FAILED;
    }
    return status;
#else
    psa_handle_t    print_handle = 0;
    print_handle = psa_connect(DRIVER_UART_SID, DRIVER_UART_VERSION);

    if (PSA_HANDLE_IS_VALID(print_handle))
    {
        status_of_call = psa_call(print_handle, 0, data1, 3, NULL, 0);
        if (status_of_call != PSA_SUCCESS)
        {
            status = VAL_STATUS_CALL_FAILED;
        }
    }
    else
    {
        return VAL_STATUS_CONNECTION_FAILED;
    }
    psa_close(print_handle);
    return status;
#endif
}

/**
 * @brief Connect to given sid
   @param  -sid : RoT service id
   @param  -version : version of RoT service
   @param  -handle - return connection handle
 * @return val_status_t
 */
STATIC_DECLARE val_status_t val_ipc_connect(uint32_t sid, uint32_t version,
                                            psa_handle_t *handle )
{
    *handle = psa_connect(sid, version);

    if (PSA_HANDLE_IS_VALID(*handle))
        return VAL_STATUS_SUCCESS;

    return VAL_STATUS_CONNECTION_FAILED;
}

/**
 * @brief Call a connected Root of Trust Service.@n
 *        The caller must provide an array of ::psa_invec_t structures as the input payload.
 * @param  handle:   Handle for the connection.
 * @param  type:     Request type.
 * @param  in_vec:   Array of psa_invec structures.
 * @param  in_len:   Number of psa_invec structures in in_vec.
 * @param  out_vec:  Array of psa_outvec structures for optional Root of Trust Service response.
 * @param  out_len:  Number of psa_outvec structures in out_vec.
 * @return val_status_t
 */
STATIC_DECLARE val_status_t val_ipc_call(psa_handle_t handle,
                                         int32_t type,
                                         const psa_invec *in_vec,
                                         size_t in_len,
                                         psa_outvec *out_vec,
                                         size_t out_len)
{
    psa_status_t call_status = PSA_SUCCESS;

    call_status = psa_call(handle, type, in_vec, in_len, out_vec, out_len);

    if (call_status != PSA_SUCCESS)
    {
        return(VAL_STATUS_CALL_FAILED);
    }

    return VAL_STATUS_SUCCESS;
}

/**
 * @brief Close a connection to a Root of Trust Service.
 *        Sends the PSA_IPC_DISCONNECT message to the Root of Trust Service so
          it can clean up resources.
 * @param handle: Handle for the connection.
 * @return void
 */
STATIC_DECLARE void val_ipc_close(psa_handle_t handle)
{
    psa_close(handle);
}

/**
 * @brief Proccess a generic connect message to given rot signal.
   @param  -sig : signal to be processed
   @param  -msg : return msg info of given signal
 * @return val_status_t.
 */
STATIC_DECLARE val_status_t val_process_connect_request(psa_signal_t sig, psa_msg_t *msg)
{
    val_status_t res = VAL_STATUS_ERROR;
    psa_signal_t signals;

wait1:
    signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (signals & sig)
    {
        if (psa_get(sig, msg) != PSA_SUCCESS)
        {
            goto wait1;
        }

        if ((msg->type != PSA_IPC_CONNECT) || (msg->handle <= 0))
        {
            val_print(PRINT_ERROR, "\tpsa_get failed for connect message\n", 0);
            res = VAL_STATUS_ERROR;
        }
        else
        {
            res = VAL_STATUS_SUCCESS;
        }
    }
    else
    {
        val_print(PRINT_ERROR, "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
        res = VAL_STATUS_ERROR;
    }
    return res;
}

/**
 * @brief Proccess a generic call message to given rot signal.
   @param  -sig : signal to be processed
   @param  -msg : return msg info of given signal
 * @return val_status_t
 */
STATIC_DECLARE val_status_t val_process_call_request(psa_signal_t sig, psa_msg_t *msg)
{
    val_status_t res = VAL_STATUS_ERROR;
    psa_signal_t signals;

wait2:
    signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (signals & sig)
    {
        if (psa_get(sig, msg) != PSA_SUCCESS)
        {
            goto wait2;
        }

        if ((msg->type < PSA_IPC_CALL) || (msg->handle <= 0))
        {
            val_print(PRINT_ERROR, "\tpsa_get failed for request message\n", 0);
            res = VAL_STATUS_ERROR;
        }
        else
        {
            res = VAL_STATUS_SUCCESS;
        }
    }
    else
    {
        val_print(PRINT_ERROR, "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
        res = VAL_STATUS_ERROR;
    }
    return res;
}

/**
 * @brief  Proccess a generic disconnect message to given rot signal.
   @param  -sig : signal to be processed
   @param  -msg : return msg info of given signal
 * @return val_status_t
 */
STATIC_DECLARE val_status_t val_process_disconnect_request(psa_signal_t sig, psa_msg_t *msg)
{
    val_status_t res = VAL_STATUS_ERROR;
    psa_signal_t signals;

wait3:
    signals = psa_wait(PSA_WAIT_ANY, PSA_BLOCK);
    if (signals & sig)
    {
        if (psa_get(sig, msg) != PSA_SUCCESS)
        {
            goto wait3;
        }

        if ((msg->type != PSA_IPC_DISCONNECT) || (msg->handle <= 0))
        {
            val_print(PRINT_ERROR, "\tpsa_get failed for disconnect massage\n", 0);
            res = VAL_STATUS_ERROR;
        }
        else
        {
            res = VAL_STATUS_SUCCESS;
        }
    }
    else
    {
        val_print(PRINT_ERROR, "\tpsa_wait returned with invalid signal value = 0x%x\n", signals);
        res = VAL_STATUS_ERROR;
    }
    return res;
}

/**
    @brief    - This function executes given list of tests from secure sequentially
                This covers secure to secure IPC API scenario
    @param    - test_info_t : test_num and block_num
    @param    - tests_list : list of tests to be executed
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_execute_secure_tests(test_info_t test_info, client_test_t *tests_list)
{
    val_status_t          status = VAL_STATUS_SUCCESS;
    val_status_t          test_status = VAL_STATUS_SUCCESS;
    psa_handle_t          handle;
    int                   i = test_info.block_num;

    while (tests_list[i] != NULL)
    {
        if (i == 1)
        val_print(PRINT_TEST, "[Info] Executing tests from secure\n", 0);

        /* Handshake with server tests */
        test_info.block_num = i;
#if STATELESS_ROT == 1
        status = val_execute_secure_test_func(&handle, test_info, SERVER_TEST_DISPATCHER_HANDLE);
        handle = (int32_t)SERVER_TEST_DISPATCHER_HANDLE;
#else
        status = val_execute_secure_test_func(&handle, test_info, SERVER_TEST_DISPATCHER_SID);
#endif
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "[Check %d] START\n", i);
            return status;
        }
        else
        {
            val_print(PRINT_DEBUG, "[Check %d] START\n", i);
        }
        /* keep track of the test block numbers, helps when the panic happened */
    	status = val_set_test_data(NV_TEST_DATA1, i);
    	if (VAL_ERROR(status))
    	{
    	   return VAL_STATUS_ERROR;
    	}
        /* Execute client tests */
        test_status = tests_list[i](CALLER_SECURE);

        /* Retrive Server test status */
        status = val_get_secure_test_result(&handle);

        status = test_status ? test_status:status;
        if (IS_TEST_SKIP(status))
        {
            val_print(PRINT_DEBUG, "[Check %d] SKIPPED\n", i);
            return status;
        }
        if (VAL_ERROR(status))
        {
            val_print(PRINT_DEBUG, "[Check %d] FAILED\n", i);
            return status;
        }
        else
        {
            val_print(PRINT_DEBUG, "[Check %d] PASSED\n", i);
        }
        i++;
    }
    return status;
}

/**
    @brief    - This function is used to handshake between:
                - nonsecure client to server test fn
                - secure client and server test fn
                - nonsecure client to secure client test fn
    @param    - handle     : handle returned while connecting given sid
    @param    - test_info  : Test_num and block_num to be executed
    @param    - sid        : RoT service to be connected. Partition dispatcher sid
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_execute_secure_test_func
               (__attribute__((unused)) psa_handle_t *handle, test_info_t test_info, uint32_t sid)
{
    uint32_t        test_data;
    val_status_t    status = VAL_STATUS_SUCCESS;
    psa_status_t    status_of_call = PSA_SUCCESS;
#if STATELESS_ROT == 1
    test_data = ((uint32_t)(test_info.test_num) | ((uint32_t)(test_info.block_num) << BLOCK_NUM_POS)
                    | ((uint32_t)(TEST_EXECUTE_FUNC) << ACTION_POS));
    psa_invec data[1] = { {&test_data, sizeof(test_data)} };

    status_of_call = psa_call(sid, 0, data, 1, NULL, 0);

    if (status_of_call != PSA_SUCCESS)
    {
    	status = VAL_STATUS_CALL_FAILED;
        val_print(PRINT_ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
    }
    return status;
#else
    *handle = psa_connect(sid, 1);

    if (*handle < 0)
    {
        val_print(PRINT_ERROR, "Could not connect SID. Handle=%x\n", *handle);
        status = VAL_STATUS_CONNECTION_FAILED;
    }

    test_data = ((uint32_t)(test_info.test_num) | ((uint32_t)(test_info.block_num) << BLOCK_NUM_POS)
                | ((uint32_t)(TEST_EXECUTE_FUNC) << ACTION_POS));
    psa_invec data[1] = {{&test_data, sizeof(test_data)}};

    status_of_call = psa_call(*handle, 0, data, 1, NULL, 0);

    if (status_of_call != PSA_SUCCESS)
    {
        status = VAL_STATUS_CALL_FAILED;
        val_print(PRINT_ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
        psa_close(*handle);
    }
    return status;
#endif
}

/**
    @brief    - This function is used to retrive the status of previously connected test function
                using val_execute_secure_test_func
    @param    - handle     : handle of server function. Handle of Partition dispatcher sid
    @return   - The status of test functions
**/
STATIC_DECLARE val_status_t val_get_secure_test_result(psa_handle_t *handle)
{
    uint32_t         test_data;
    val_status_t     status = VAL_STATUS_SUCCESS;
    psa_status_t     status_of_call = PSA_SUCCESS;

    test_data = (TEST_RETURN_RESULT << ACTION_POS);

    psa_outvec resp = {&status, sizeof(status)};
    psa_invec data[1] = {{&test_data, sizeof(test_data)}};

    status_of_call = psa_call(*handle, 0, data, 1, &resp, 1);
    if (status_of_call != PSA_SUCCESS)
    {
        status = VAL_STATUS_CALL_FAILED;
        val_print(PRINT_ERROR, "Call to dispatch SF failed. Status=%x\n", status_of_call);
    }
#if STATELESS_ROT != 1
    psa_close(*handle);
#endif
    return status;
}

/*
    @brief           - This function checks if the input status argument is an error.
                       On error, print the checkpoint value
    @param           - checkpoint      : Test debug checkpoint
                     - val_status_t   : Test status
    @return          - returns the input status back to the program.
*/
STATIC_DECLARE val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status)
{
    if (VAL_ERROR(status))
    {
        val_print(PRINT_ERROR, "\tCheckpoint %d : ", checkpoint);
        val_print(PRINT_ERROR, "Error Code=0x%x \n", status);
    }
    else
    {
        val_print(PRINT_DEBUG, "\tCheckpoint %d \n", checkpoint);
    }
    return status;
}

/*
    @brief     - Writes 'size' bytes from buffer into non-volatile memory at a given
                'base + offset'. This is client interface API of secure partition
                val_nvmem_write_sf API for spe world
               - offset    : Offset
               - buffer    : Pointer to source address
               - size      : Number of bytes
    @return    - val_status_t
*/
STATIC_DECLARE val_status_t val_nvmem_write(uint32_t offset, void *buffer, int size)
{
   nvmem_param_t   nvmem_param;

   psa_status_t    status_of_call = PSA_SUCCESS;
   val_status_t    status = VAL_STATUS_SUCCESS;
   memory_desc_t   *memory_desc;

   status = val_target_get_config(TARGET_CONFIG_CREATE_ID(GROUP_MEMORY, MEMORY_NVMEM, 0),
                                 (uint8_t **)&memory_desc,
                                 (uint32_t *)sizeof(memory_desc_t));

   if (VAL_ERROR(status))
   {
      return status;
   }

   nvmem_param.nvmem_fn_type = NVMEM_WRITE;
   nvmem_param.base = memory_desc->start;
   nvmem_param.offset = offset;
   nvmem_param.size = size;
   psa_invec invec[2] = {{&nvmem_param, sizeof(nvmem_param)}, {buffer, size}};

#if STATELESS_ROT == 1
   status_of_call = psa_call(DRIVER_NVMEM_HANDLE, 0, invec, 2, NULL, 0);
   if (status_of_call != PSA_SUCCESS)
   {
	   return VAL_STATUS_CALL_FAILED;
   }
   return VAL_STATUS_SUCCESS;

#else
   psa_handle_t    handle = 0;
   handle = psa_connect(DRIVER_NVMEM_SID, DRIVER_NVMEM_VERSION);
   if (PSA_HANDLE_IS_VALID(handle))
   {
       status_of_call = psa_call(handle, 0, invec, 2, NULL, 0);
       if (status_of_call != PSA_SUCCESS)
       {
           psa_close(handle);
           return VAL_STATUS_CALL_FAILED;
       }
   }
   else
   {
       return VAL_STATUS_CONNECTION_FAILED;
   }
   psa_close(handle);
   return VAL_STATUS_SUCCESS;
#endif
}

/**
    @brief    - This function sets the given boot.state value to corresponding
                boot NVMEM location
    @param    - state: boot_state_t
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_set_boot_flag(boot_state_t state)
{
   boot_t           boot;
   val_status_t     status;

   boot.state = state;
   status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_BOOT), &boot, sizeof(boot_t));
   if (VAL_ERROR(status))
   {
       val_print(PRINT_ERROR, "\tval_nvmem_write failed Error=0x%x\n", status);
       return status;
   }
   return status;
}

/**
    @brief    - This function sets the test specific data
                NVMEM location
    @param    - nvm index
    @param    - nvm testdata
    @return   - val_status_t
**/
STATIC_DECLARE val_status_t val_set_test_data(int32_t nvm_index, int32_t test_data)
{
   val_status_t     status;

   status = val_nvmem_write(VAL_NVMEM_OFFSET(nvm_index), &test_data, sizeof(int32_t));
   if (VAL_ERROR(status))
   {
       val_print(PRINT_ERROR, "\tval_nvmem_write failed for test data. Error=0x%x\n", status);
       return status;
   }
   return status;
}


#endif
