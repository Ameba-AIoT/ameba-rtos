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

#include "val_client_defs.h"
#include "val_service_defs.h"

#define val CONCAT(val, _server_sp)
#define psa CONCAT(psa, _server_sp)
extern val_api_t *val;
extern psa_api_t *psa;

#if STATELESS_ROT == 1

int32_t server_test_call_read_and_skip(void);
int32_t server_test_call_and_write(void);
int32_t server_test_zero_length_invec(void);
int32_t server_test_zero_length_outvec(void);
int32_t server_test_overlapping_vectors(void);

const server_test_t test_i003_server_tests_list[] = {
    NULL,
    server_test_zero_length_invec,
    server_test_zero_length_outvec,
    server_test_call_read_and_skip,
    server_test_call_and_write,
    server_test_overlapping_vectors,
    NULL,
};

static void exit_graceful(psa_handle_t msg_handle, int status_code,
                int print_next_args, int expected_data, int actual_data)
{
    if (print_next_args != 0)
    {
        val->print(PRINT_ERROR, "\tExpected data=%x\n", expected_data);
        val->print(PRINT_ERROR, "\tActual data=%x\n", actual_data);
    }
    /* Negative status_code represents check failure and each check has
     * uniq status_code to identify failing point
     */
    psa->reply(msg_handle, status_code);
}

int32_t server_test_zero_length_invec(void)
{
    int32_t              status = VAL_STATUS_SUCCESS;
    psa_msg_t            msg = {0};
    int                  data[5] = {0}, actual_data = 0x22;

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.in_size[0] != 0)) ||
        ((msg.in_size[1] != 0)) ||
        ((msg.in_size[2] != sizeof(int))) ||
        ((msg.in_size[3] != 0)))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    if ((msg.in_size[2] <= sizeof(data[2])) &&
        (psa->read(msg.handle, 2, &data[2], msg.in_size[2]) != msg.in_size[2]))
    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_READ_FAILED;
    }

    if (actual_data != data[2])
    {
        exit_graceful(msg.handle, -5, 1, data[2], actual_data);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.out_size[0] != sizeof(int))) ||
        ((msg.out_size[1] != 0))           ||
        ((msg.out_size[2] != 0))           ||
        ((msg.out_size[3] != 0)))
    {
         exit_graceful(msg.handle, -6, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }

    psa->write(msg.handle, 0, &data[2], msg.out_size[0]);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_zero_length_outvec(void)
{
    int32_t              status = VAL_STATUS_SUCCESS;
    psa_msg_t            msg = {0};
    int                  data[5] = {0}, actual_data = 0x11;

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.in_size[0] != sizeof(int))) ||
        ((msg.in_size[1] != 0)) ||
        ((msg.in_size[2] != 0)) ||
        ((msg.in_size[3] != 0)))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    if ((msg.in_size[0] <= sizeof(data[0])) &&
        (psa->read(msg.handle, 0, &data[0], msg.in_size[0]) != msg.in_size[0]))
    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_READ_FAILED;
    }

    if (actual_data != data[0])
    {
        exit_graceful(msg.handle, -5, 1, data[2], actual_data);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.out_size[0] != 0))            ||
        ((msg.out_size[1] != 0))           ||
        ((msg.out_size[2] != sizeof(int))) ||
        ((msg.out_size[3] != 0)))
    {
         exit_graceful(msg.handle, -6, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }
    psa->write(msg.handle, 2, &data[0], msg.out_size[2]);

    /* Dummy write with zero byte. This should not overwrite previously written data */
    psa->write(msg.handle, 2, &data[0], msg.out_size[0]);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_call_read_and_skip(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    int         expected_data1[2] = {0xaa, 0xbb};
    int         expected_data2[4] = {0x7788, 0x2233, 0x2211, 0x50607080},
                actual_data[4] = {0}, i;
    psa_msg_t   msg = {0};

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    /* msg.in_size should match with the size of psa_invec given to psa_call*/
    if (((msg.in_size[0] != sizeof(int))) ||
        ((msg.in_size[1] != sizeof(int))) ||
        ((msg.in_size[2] != sizeof(uint64_t))) ||
        ((msg.in_size[3] != sizeof(uint64_t))))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    /* Full size read for invec 0 and invec 1 */
    for (i = 0; i < 2 ; i++)
    {
       if ((msg.in_size[i] <= sizeof(actual_data[i])) &&
           (psa->read(msg.handle, i, &actual_data[i], msg.in_size[i]) != msg.in_size[i]))
       {
           exit_graceful(msg.handle, -4, 0, 0, 0);
           return VAL_STATUS_READ_FAILED;
       }
       if (actual_data[i] != expected_data1[i])
       {
           exit_graceful(msg.handle, -5, 1, expected_data1[i], actual_data[i]);
           return VAL_STATUS_READ_FAILED;
       }
    }

    /* Inbound read of 2 bytes from invec 2 */
    if (psa->read(msg.handle, 2, &actual_data[0], 2) != 2)
    {
       exit_graceful(msg.handle, -6, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }
    if (actual_data[0] != expected_data2[0])
    {
       exit_graceful(msg.handle, -7, 1, expected_data2[0], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Inbound read of 3 bytes from invec 2 */
    if (psa->skip(msg.handle, 2, 3) != 3)
    {
       exit_graceful(msg.handle, -8, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Check previous psa_skip has actually skipped 3 bytes */
    if (psa->read(msg.handle, 2, &actual_data[0], 2) != 2)
    {
       exit_graceful(msg.handle, -9, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    if (actual_data[0] != expected_data2[1])
    {
       exit_graceful(msg.handle, -10, 1, expected_data2[1], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Outbound read of 3 bytes from invec 2
     * Only one byte should be updated in buffer. Remaining space should be untouched
     */
    if (psa->read(msg.handle, 2, &actual_data[0], 3) != 1)
    {
       exit_graceful(msg.handle, -11, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    if (actual_data[0] != expected_data2[2])
    {
       exit_graceful(msg.handle, -12, 1, expected_data2[2], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    actual_data[0] = 0xaa;

    /* After outbound read, subsequent read or skip to invec 2 should return 0
     * and memory buffer shouldn't be updated
     */
    if ((psa->read(msg.handle, 2, &actual_data[0], 3) != 0) ||
        (psa->skip(msg.handle, 2, 3) != 0) || (actual_data[0] != 0xaa))
    {
       exit_graceful(msg.handle, -13, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    /* Read of zero bytes should not read anything */
    if ((psa->read(msg.handle, 3, &actual_data[0], 0) != 0) || (actual_data[0] != 0xaa))
    {
       exit_graceful(msg.handle, -14, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    /* Skip of zero bytes should not skip anything */
    if (psa->skip(msg.handle, 3, 0) != 0)
    {
       exit_graceful(msg.handle, -15, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Check effect of previous zero byte read and skip */
    psa->read(msg.handle, 3, &actual_data[0], 4);
    if (actual_data[0] != expected_data2[3])
    {
       exit_graceful(msg.handle, -16, 1, expected_data2[3], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Outbound skip to invec 3 */
    if (psa->skip(msg.handle, 3, 5) != 4)
    {
       exit_graceful(msg.handle, -17, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }
    if (psa->skip(msg.handle, 3, 5) != 0)
    {
       exit_graceful(msg.handle, -18, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Since out_len is 0, msg.out_size should be 0 */
    if ((msg.out_size[0] != 0)  ||
        (msg.out_size[1] != 0) ||
        (msg.out_size[2] != 0) ||
        (msg.out_size[3] != 0))
    {
         exit_graceful(msg.handle, -14, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_call_and_write(void)
{
    int32_t      status = VAL_STATUS_SUCCESS;
    int          data[5] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee}, i;
    psa_msg_t    msg = {0};

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    /* msg.out_size should match with size of psa_outvec given to psa_call */
    if (((msg.out_size[0] != sizeof(int))) ||
        ((msg.out_size[1] != sizeof(int))) ||
        ((msg.out_size[2] != sizeof(int))) ||
        ((msg.out_size[3] != sizeof(int))))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }

    /* Since in_len=0, all msg.in_size should be 0 */
    if ((msg.in_size[0] != 0) ||
        (msg.in_size[1] != 0) ||
        (msg.in_size[2] != 0) ||
        (msg.in_size[3] != 0))

    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    for (i = 0; i < 3 ; i++)
    {
       psa->write(msg.handle, i, &data[i], msg.out_size[i]);
    }

    /* Zero byte write shouldn't have any effect */
    psa->write(msg.handle, 3, &data[3], 0);

    /*Using invec 3 to test write concatenation behaviour */
    psa->write(msg.handle, 3, &data[3], 1);
    psa->write(msg.handle, 3, &data[4], 1);

    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_overlapping_vectors(void)
{
    int32_t       status = VAL_STATUS_SUCCESS;
    psa_msg_t     msg = {0};
    uint8_t       wr_data[] = {0x22, 0x33},
                  rd_data[] = {0x0, 0x0},
                  expected_data[] = {0x11, 0x22};

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(208), status))
    {
        exit_graceful(msg.handle, -1, 0, 0, 0);
        return status;
    }

     /* Performing read after write to overlapping vector. */
    psa->write(msg.handle, 0, &wr_data[0], 1);
    psa->read(msg.handle, 0, &rd_data[0], 1);

    /* rd_data[0] should either be original value or modified value */
    if ((rd_data[0] != expected_data[0]) && (rd_data[0] != expected_data[1]))
    {
        val->print(PRINT_ERROR, "\tReceived invalid data=%x\n", rd_data[0]);
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return status;
    }

     /* Performing write after write to overlapping vector. */
    psa->write(msg.handle, 1, &wr_data[1], 1);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

#else

int32_t server_test_psa_set_rhandle(void);
int32_t server_test_call_read_and_skip(void);
int32_t server_test_call_and_write(void);
int32_t server_test_zero_length_invec(void);
int32_t server_test_zero_length_outvec(void);
int32_t server_test_overlapping_vectors(void);

const server_test_t test_i003_server_tests_list[] = {
    NULL,
    server_test_zero_length_invec,
    server_test_zero_length_outvec,
    server_test_call_read_and_skip,
    server_test_call_and_write,
    server_test_psa_set_rhandle,
    server_test_overlapping_vectors,
    NULL,
};

static void exit_graceful(psa_handle_t msg_handle, int status_code,
                int print_next_args, int expected_data, int actual_data)
{
    psa_msg_t  msg={0};

    if (print_next_args != 0)
    {
        val->print(PRINT_ERROR, "\tExpected data=%x\n", expected_data);
        val->print(PRINT_ERROR, "\tActual data=%x\n", actual_data);
    }
    /* Negative status_code represents check failure and each check has
     * uniq status_code to identify failing point
     */
    psa->reply(msg_handle, status_code);

    if (val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        val->print(PRINT_ERROR, "\tdisconnect failed in exit_graceful func\n", 0);
    }
    psa->reply(msg.handle, PSA_SUCCESS);
}

int32_t server_test_zero_length_invec(void)
{
    int32_t              status = VAL_STATUS_SUCCESS;
    psa_msg_t            msg = {0};
    int                  data[5] = {0}, actual_data = 0x22;

    if (val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return VAL_STATUS_CONNECTION_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.in_size[0] != 0)) ||
        ((msg.in_size[1] != 0)) ||
        ((msg.in_size[2] != sizeof(int))) ||
        ((msg.in_size[3] != 0)))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    if ((msg.in_size[2] <= sizeof(data[2])) &&
        (psa->read(msg.handle, 2, &data[2], msg.in_size[2]) != msg.in_size[2]))
    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_READ_FAILED;
    }

    if (actual_data != data[2])
    {
        exit_graceful(msg.handle, -5, 1, data[2], actual_data);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.out_size[0] != sizeof(int))) ||
        ((msg.out_size[1] != 0))           ||
        ((msg.out_size[2] != 0))           ||
        ((msg.out_size[3] != 0)))
    {
         exit_graceful(msg.handle, -6, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }

    psa->write(msg.handle, 0, &data[2], msg.out_size[0]);
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_zero_length_outvec(void)
{
    int32_t              status=VAL_STATUS_SUCCESS;
    psa_msg_t            msg={0};
    int                  data[5] ={0}, actual_data=0x11;

    if (val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return VAL_STATUS_CONNECTION_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.in_size[0] != sizeof(int))) ||
        ((msg.in_size[1] != 0)) ||
        ((msg.in_size[2] != 0)) ||
        ((msg.in_size[3] != 0)))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    if ((msg.in_size[0] <= sizeof(data[0])) &&
        (psa->read(msg.handle, 0, &data[0], msg.in_size[0]) != msg.in_size[0]))
    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_READ_FAILED;
    }

    if (actual_data != data[0])
    {
        exit_graceful(msg.handle, -5, 1, data[2], actual_data);
        return VAL_STATUS_CALL_FAILED;
    }

    if (((msg.out_size[0] != 0))            ||
        ((msg.out_size[1] != 0))           ||
        ((msg.out_size[2] != sizeof(int))) ||
        ((msg.out_size[3] != 0)))
    {
         exit_graceful(msg.handle, -6, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }
    psa->write(msg.handle, 2, &data[0], msg.out_size[2]);

    /* Dummy write with zero byte. This should not overwrite previously written data */
    psa->write(msg.handle, 2, &data[0], msg.out_size[0]);
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_call_read_and_skip(void)
{
    int32_t     status = VAL_STATUS_SUCCESS;
    int         expected_data1[2] = {0xaa, 0xbb};
    int         expected_data2[4] = {0x7788, 0x2233, 0x2211, 0x50607080},
                actual_data[4] = {0}, i;
    psa_msg_t   msg = {0};

    if (val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return VAL_STATUS_CONNECTION_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    /* msg.in_size should match with the size of psa_invec given to psa_call*/
    if (((msg.in_size[0] != sizeof(int))) ||
        ((msg.in_size[1] != sizeof(int))) ||
        ((msg.in_size[2] != sizeof(uint64_t))) ||
        ((msg.in_size[3] != sizeof(uint64_t))))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    /* Full size read for invec 0 and invec 1 */
    for (i = 0; i < 2 ; i++)
    {
       if ((msg.in_size[i] <= sizeof(actual_data[i])) &&
           (psa->read(msg.handle, i, &actual_data[i], msg.in_size[i]) != msg.in_size[i]))
       {
           exit_graceful(msg.handle, -4, 0, 0, 0);
           return VAL_STATUS_READ_FAILED;
       }
       if (actual_data[i] != expected_data1[i])
       {
           exit_graceful(msg.handle, -5, 1, expected_data1[i], actual_data[i]);
           return VAL_STATUS_READ_FAILED;
       }
    }

    /* Inbound read of 2 bytes from invec 2 */
    if (psa->read(msg.handle, 2, &actual_data[0], 2) != 2)
    {
       exit_graceful(msg.handle, -6, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }
    if (actual_data[0] != expected_data2[0])
    {
       exit_graceful(msg.handle, -7, 1, expected_data2[0], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Inbound read of 3 bytes from invec 2 */
    if (psa->skip(msg.handle, 2, 3) != 3)
    {
       exit_graceful(msg.handle, -8, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Check previous psa_skip has actually skipped 3 bytes */
    if (psa->read(msg.handle, 2, &actual_data[0], 2) != 2)
    {
       exit_graceful(msg.handle, -9, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    if (actual_data[0] != expected_data2[1])
    {
       exit_graceful(msg.handle, -10, 1, expected_data2[1], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Outbound read of 3 bytes from invec 2
     * Only one byte should be updated in buffer. Remaining space should be untouched
     */
    if (psa->read(msg.handle, 2, &actual_data[0], 3) != 1)
    {
       exit_graceful(msg.handle, -11, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    if (actual_data[0] != expected_data2[2])
    {
       exit_graceful(msg.handle, -12, 1, expected_data2[2], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    actual_data[0] = 0xaa;

    /* After outbound read, subsequent read or skip to invec 2 should return 0
     * and memory buffer shouldn't be updated
     */
    if ((psa->read(msg.handle, 2, &actual_data[0], 3) != 0) ||
        (psa->skip(msg.handle, 2, 3) != 0) || (actual_data[0] != 0xaa))
    {
       exit_graceful(msg.handle, -13, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    /* Read of zero bytes should not read anything */
    if ((psa->read(msg.handle, 3, &actual_data[0], 0) != 0) || (actual_data[0] != 0xaa))
    {
       exit_graceful(msg.handle, -14, 0, 0, 0);
       return VAL_STATUS_READ_FAILED;
    }

    /* Skip of zero bytes should not skip anything */
    if (psa->skip(msg.handle, 3, 0) != 0)
    {
       exit_graceful(msg.handle, -15, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Check effect of previous zero byte read and skip */
    psa->read(msg.handle, 3, &actual_data[0], 4);
    if (actual_data[0] != expected_data2[3])
    {
       exit_graceful(msg.handle, -16, 1, expected_data2[3], actual_data[0]);
       return VAL_STATUS_READ_FAILED;
    }

    /* Outbound skip to invec 3 */
    if (psa->skip(msg.handle, 3, 5) != 4)
    {
       exit_graceful(msg.handle, -17, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }
    if (psa->skip(msg.handle, 3, 5) != 0)
    {
       exit_graceful(msg.handle, -18, 0, 0, 0);
       return VAL_STATUS_SKIP_FAILED;
    }

    /* Since out_len is 0, msg.out_size should be 0 */
    if ((msg.out_size[0] != 0)  ||
        (msg.out_size[1] != 0) ||
        (msg.out_size[2] != 0) ||
        (msg.out_size[3] != 0))
    {
         exit_graceful(msg.handle, -14, 0, 0, 0);
         return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_call_and_write(void)
{
    int32_t      status = VAL_STATUS_SUCCESS;
    int          data[5] = {0xaa, 0xbb, 0xcc, 0xdd, 0xee}, i;
    psa_msg_t    msg = {0};

    if (val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return VAL_STATUS_CONNECTION_FAILED;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    if (val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return VAL_STATUS_CALL_FAILED;
    }

    /* msg.out_size should match with size of psa_outvec given to psa_call */
    if (((msg.out_size[0] != sizeof(int))) ||
        ((msg.out_size[1] != sizeof(int))) ||
        ((msg.out_size[2] != sizeof(int))) ||
        ((msg.out_size[3] != sizeof(int))))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return VAL_STATUS_MSG_OUTSIZE_FAILED;
    }

    /* Since in_len=0, all msg.in_size should be 0 */
    if ((msg.in_size[0] != 0) ||
        (msg.in_size[1] != 0) ||
        (msg.in_size[2] != 0) ||
        (msg.in_size[3] != 0))

    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return VAL_STATUS_MSG_INSIZE_FAILED;
    }

    for (i = 0; i < 3 ; i++)
    {
       psa->write(msg.handle, i, &data[i], msg.out_size[i]);
    }

    /* Zero byte write shouldn't have any effect */
    psa->write(msg.handle, 3, &data[3], 0);

    /*Using invec 3 to test write concatenation behaviour */
    psa->write(msg.handle, 3, &data[3], 1);
    psa->write(msg.handle, 3, &data[4], 1);

    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

int32_t server_test_psa_set_rhandle(void)
{
    int32_t       status = VAL_STATUS_SUCCESS;
    psa_msg_t     msg = {0};
    int           num;

    /*rhandle value check when PSA_IPC_CONNECT */
    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (msg.rhandle != NULL)
    {
        status = VAL_STATUS_INVALID_HANDLE;
    }
    if (val->err_check_set(TEST_CHECKPOINT_NUM(201), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);


    /*rhandle value check when PSA_IPC_CALL */
    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (msg.rhandle != NULL)
    {
        status = VAL_STATUS_INVALID_HANDLE;
    }
    if (val->err_check_set(TEST_CHECKPOINT_NUM(202), status))
    {
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return status;
    }

    /*set rhandle */
    num = 5;
    psa->set_rhandle(msg.handle, &num);
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (*(int *)(msg.rhandle) != num)
    {
        status = VAL_STATUS_INVALID_HANDLE;
    }
    if (val->err_check_set(TEST_CHECKPOINT_NUM(203), status))
    {
        exit_graceful(msg.handle, -3, 0, 0, 0);
        return status;
    }

    /*set rhandle other than previous value to see effect of
     * updated value in next msg delivery. Next msg should
     * return the updated value.
     */
    num = 10;
    psa->set_rhandle(msg.handle, &num);
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (*(int *)(msg.rhandle) != num)
    {
        status = VAL_STATUS_INVALID_HANDLE;
    }
    if (val->err_check_set(TEST_CHECKPOINT_NUM(204), status))
    {
        exit_graceful(msg.handle, -4, 0, 0, 0);
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    /* rhandle should retain the value at PSA_IPC_DISCONNECT too */
    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (*(int *)(msg.rhandle) != num)
    {
        status = VAL_STATUS_INVALID_HANDLE;
    }
    val->err_check_set(TEST_CHECKPOINT_NUM(205), status);

    /*Setting the rhandle for a connection during disconnection has no observable effect*/
    num = 15;
    psa->set_rhandle(msg.handle, &num);

    /* Sanity check - previous call shouldn't hang */
    val->err_check_set(TEST_CHECKPOINT_NUM(206), status);
    psa->reply(msg.handle, PSA_SUCCESS);
    return status;
}

int32_t server_test_overlapping_vectors(void)
{
    int32_t       status = VAL_STATUS_SUCCESS;
    psa_msg_t     msg = {0};
    uint8_t       wr_data[] = {0x22, 0x33},
                  rd_data[] = {0x0, 0x0},
                  expected_data[] = {0x11, 0x22};

    status = val->process_connect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(207), status))
    {
        psa->reply(msg.handle, PSA_ERROR_CONNECTION_REFUSED);
        return status;
    }
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_call_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    if (val->err_check_set(TEST_CHECKPOINT_NUM(208), status))
    {
        exit_graceful(msg.handle, -1, 0, 0, 0);
        return status;
    }

     /* Performing read after write to overlapping vector. */
    psa->write(msg.handle, 0, &wr_data[0], 1);
    psa->read(msg.handle, 0, &rd_data[0], 1);

    /* rd_data[0] should either be original value or modified value */
    if ((rd_data[0] != expected_data[0]) && (rd_data[0] != expected_data[1]))
    {
        val->print(PRINT_ERROR, "\tReceived invalid data=%x\n", rd_data[0]);
        exit_graceful(msg.handle, -2, 0, 0, 0);
        return status;
    }

     /* Performing write after write to overlapping vector. */
    psa->write(msg.handle, 1, &wr_data[1], 1);
    psa->reply(msg.handle, PSA_SUCCESS);

    status = val->process_disconnect_request(SERVER_UNSPECIFED_VERSION_SIGNAL, &msg);
    val->err_check_set(TEST_CHECKPOINT_NUM(209), status);
    psa->reply(msg.handle, PSA_SUCCESS);

    return status;
}

#endif