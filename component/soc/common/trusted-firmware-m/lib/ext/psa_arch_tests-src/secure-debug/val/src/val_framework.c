/** @file
 * Copyright (c) 2021 Arm Limited or its affiliates. All rights reserved.
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

#include "val_framework.h"
#include "val_interfaces.h"
#include "val_dispatcher.h"
#include "val_peripherals.h"

extern val_api_t val_api;

/* globals */
test_status_buffer_t    g_status_buffer;

/**
    @brief    - Parses input status for a given test and
                outputs appropriate information on the console
    @return   - Test state
**/
uint32_t val_report_status(void)
{
    uint32_t status, state;

    status = val_get_status();

    state = (status >> TEST_STATE_BIT) & TEST_STATE_MASK;
    status = status & TEST_STATUS_MASK;

    switch (state)
    {
        case TEST_START:
            state = TEST_FAIL;
            val_print(PRINT_ALWAYS, "\nTEST RESULT: FAILED (Error Code=0x%x)\n",
                                                    VAL_STATUS_INIT_FAILED);
            break;

        case TEST_END:
            state = TEST_PASS;
            val_print(PRINT_ALWAYS, "\nTEST RESULT: PASSED\n", 0);
            break;

        case TEST_FAIL:
            val_print(PRINT_ALWAYS, "\nTEST RESULT: FAILED (Error Code=0x%x)\n", status);
            break;

        case TEST_SKIP:
            state = TEST_SKIP;
            val_print(PRINT_ALWAYS, "\nTEST RESULT: SKIPPED (Skip Code=0x%x)\n", status);
            break;

        case TEST_PENDING:
            val_print(PRINT_ALWAYS, "\nTEST RESULT: SIM ERROR (Error Code=0x%x)\n", status);
            break;

        default:
            state = TEST_FAIL;
            val_print(PRINT_ALWAYS, "\nTEST RESULT: FAILED(Error Code=0x%x)\n", VAL_STATUS_INVALID);
            break;
    }

    val_print(PRINT_ALWAYS, "\n******************************************\n", 0);
    return state;
}

/**
    @brief    - Records the state and status of test
    @return   - val_status_t
**/
val_status_t val_set_status(uint32_t status)
{
    g_status_buffer.state   = ((status >> TEST_STATE_BIT) & TEST_STATE_MASK);
    g_status_buffer.status  = (status & TEST_STATUS_MASK);

    return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Updates the state and status for a given test
    @return   - test status
**/
uint32_t val_get_status(void)
{
    return ((g_status_buffer.state) << TEST_STATE_BIT) | (g_status_buffer.status);
}

/*
    @brief           - This function checks if the input status argument is an error.
                       On error, we print the checkpoint value and set the status.
    @param           - checkpoint     : Test debug checkpoint
                     - val_status_t   : Test status
    @return          - returns the input status back to the program.
*/

val_status_t val_err_check_set(uint32_t checkpoint, val_status_t status)
{
    if (VAL_ERROR(status)) {
        val_print(PRINT_ERROR, "\tCheckpoint %d : ", checkpoint);
        val_print(PRINT_ERROR, "Error Code=0x%x \n", status);
        val_set_status(RESULT_FAIL(status));
    } else {
        status = (val_get_status() & TEST_STATUS_MASK);
        if (VAL_ERROR(status)) {
            val_print(PRINT_ERROR, "\tCheckpoint %d : ", checkpoint);
            val_print(PRINT_ERROR, "Error Code=0x%x \n", status);
        } else
            val_print(PRINT_DEBUG, "\tCheckpoint %d \n", checkpoint);
    }
    return status;
}

/**
  @brief  This API prints the test number, description and
          sets the test state to TEST_START on successful execution.
  @param  test_num       :unique number identifying this test
  @param  desc           :brief description of the test
  @param  test_bitfield  :Addition test info such as
                           - test isolation level requirement
                           - Watchdog timeout type
  @return void
**/

void val_test_init(uint32_t test_num, char8_t *desc)
{
   val_status_t         status = VAL_STATUS_SUCCESS;

   /*global init*/
   g_status_buffer.state   = TEST_FAIL;
   g_status_buffer.status  = VAL_STATUS_INVALID;

   val_print(PRINT_ALWAYS, "\nTEST: %d | DESCRIPTION: ", test_num);
   val_print(PRINT_ALWAYS, desc, 0);

   val_set_status(RESULT_START(status));
   return;
}

/**
  @brief  This API sets the test state to TEST_END if test is successfully passed.
  @param none
  @return none
**/

void val_test_exit(void)
{
    val_status_t status = VAL_STATUS_SUCCESS;

    status = val_get_status();

    /* return if test skipped or failed */
    if (IS_TEST_FAIL(status) || IS_TEST_SKIP(status))
        return;
    else
        val_set_status(RESULT_END(VAL_STATUS_SUCCESS));
}
