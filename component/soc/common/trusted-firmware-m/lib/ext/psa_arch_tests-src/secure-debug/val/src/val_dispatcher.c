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
#include "val_dispatcher.h"
#include "val_interfaces.h"
#include "val_peripherals.h"

extern val_api_t val_api;

/* globals */
addr_t          g_test_info_addr;

/**
    @brief        - This function prints PSA_{SUITE}_API_VERSION_MAJOR
                    PSA_{SUITE}_API_VERSION_MINOR details.
    @param        - None
    @return       - None
**/
static void val_print_api_version(void)
{
#ifdef CRYPTO
    val_print(PRINT_ALWAYS, " %d.", PSA_CRYPTO_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_CRYPTO_API_VERSION_MINOR);
#endif
#ifdef INTERNAL_TRUSTED_STORAGE
    val_print(PRINT_ALWAYS, " %d.", PSA_ITS_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_ITS_API_VERSION_MINOR);
#endif
#ifdef PROTECTED_STORAGE
    val_print(PRINT_ALWAYS, " %d.", PSA_PS_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_PS_API_VERSION_MINOR);
#endif
#ifdef STORAGE
    val_print(PRINT_ALWAYS, " ITS %d.", PSA_ITS_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_ITS_API_VERSION_MINOR);
    val_print(PRINT_ALWAYS, " and PS %d.", PSA_PS_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_PS_API_VERSION_MINOR);
#endif
#ifdef INITIAL_ATTESTATION
    val_print(PRINT_ALWAYS, " %d.", PSA_INITIAL_ATTEST_API_VERSION_MAJOR);
    val_print(PRINT_ALWAYS, "%d", PSA_INITIAL_ATTEST_API_VERSION_MINOR);
#endif
}

/**
    @brief        - This function reads the test ELFs from RAM or secondary storage and loads into
                    system memory
    @param        - test_id      : Returns the current test ID
                  - test_id_prev : Previous test ID.
    @return       - Error code
**/
val_status_t val_test_load(test_id_t *test_id, test_id_t test_id_prev)
{
    int             i;
    val_test_info_t test_list[] = {
#include "test_entry_list.inc"
                                  {VAL_INVALID_TEST_ID, NULL}
                                  };

    for (i = 0; i < (int)(sizeof(test_list)/sizeof(test_list[0])); i++)
    {
        if (test_id_prev == VAL_INVALID_TEST_ID)
        {
            *test_id = test_list[i].test_id;
            g_test_info_addr = (addr_t) test_list[i].entry_addr;
            return VAL_STATUS_SUCCESS;
        }
        else if (test_id_prev == test_list[i].test_id)
        {
            *test_id = test_list[i+1].test_id;
            g_test_info_addr = (addr_t) test_list[i+1].entry_addr;
            return VAL_STATUS_SUCCESS;
        }
        else if (test_list[i].test_id == VAL_INVALID_TEST_ID)
        {
            val_print(PRINT_DEBUG, "\n\nNo more valid tests found. Exiting.", 0);
            *test_id = VAL_INVALID_TEST_ID;
            return VAL_STATUS_SUCCESS;
        }
    }

    *test_id = VAL_INVALID_TEST_ID;
    val_print(PRINT_ERROR, "\n\nError: No more valid tests found. Exiting.", 0);
    return VAL_STATUS_LOAD_ERROR;
}

/**
    @brief    - This function reads the function pointer addresses for
                test_entry
    @param    - paddr    : Returns the Test function address
    @return   - Returns val_status_t
**/
val_status_t val_get_test_entry_addr(addr_t *paddr)
{
    *paddr = g_test_info_addr;
    return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Execute the function pointer which was given to us by the test
    @param    - void
**/
void val_execute_test_fn(void)
{
    test_fptr_t   fn_ptr;
    addr_t        addr;

    val_get_test_entry_addr(&addr);
    fn_ptr = (test_fptr_t)addr;
    fn_ptr(&val_api);
    return;
}

/*
    @brief    - Reads the pre-defined component name against given test_id
    @param    - test_id  : Current Test ID
    @return   - Component name
*/
char *val_get_comp_name(test_id_t test_id)
{
    switch (VAL_GET_COMP_NUM(test_id))
    {
        case VAL_SECURE_DEBUG_BASE:
            return "Secure Debug Suite";
        default:
            return "Unknown Suite";
    }
}

/**
    @brief    - This function is responsible for setting up VAL infrastructure.
                Loads test one by one from combine binary and calls test_entry
                function of each test image.
    @return   - 0 if success Or error code for the failure.
**/
int32_t val_dispatcher(test_id_t test_id_prev)
{
    test_id_t            test_id;
    val_status_t         status;
    test_count_t         test_count = {0,};
    uint32_t             test_result;

    do
    {
		status = val_test_load(&test_id, test_id_prev);

		if (VAL_ERROR(status))
		{
			return status;
		}
		else if (test_id == VAL_INVALID_TEST_ID)
		{
			break;
		}
		if (VAL_GET_COMP_NUM(test_id_prev) != VAL_GET_COMP_NUM(test_id))
		{
			val_print(PRINT_ALWAYS, "\nRunning.. ", 0);
			val_print(PRINT_ALWAYS, val_get_comp_name(test_id), 0);
			val_print(PRINT_ALWAYS, "\n******************************\n", 0);
		}

		val_execute_test_fn();

        test_result = val_report_status();

        switch (test_result)
        {
            case TEST_PASS:
                test_count.pass_cnt += 1;
                break;
            case TEST_FAIL:
                test_count.fail_cnt += 1;
                break;
            case TEST_SKIP:
                test_count.skip_cnt += 1;
                break;
            case TEST_PENDING:
                test_count.sim_error_cnt += 1;
                break;
        }

        test_id_prev = test_id;
   } while (1);

   val_print(PRINT_ALWAYS, "\n************ ", 0);
   val_print(PRINT_ALWAYS, val_get_comp_name(test_id_prev), 0);
   val_print(PRINT_ALWAYS, " Report **********\n", 0);
   val_print(PRINT_ALWAYS, "TOTAL TESTS     : %d\n", test_count.pass_cnt + test_count.fail_cnt
            + test_count.skip_cnt + test_count.sim_error_cnt);
   val_print(PRINT_ALWAYS, "TOTAL PASSED    : %d\n", test_count.pass_cnt);
   val_print(PRINT_ALWAYS, "TOTAL SIM ERROR : %d\n", test_count.sim_error_cnt);
   val_print(PRINT_ALWAYS, "TOTAL FAILED    : %d\n", test_count.fail_cnt);
   val_print(PRINT_ALWAYS, "TOTAL SKIPPED   : %d\n", test_count.skip_cnt);
   val_print(PRINT_ALWAYS, "******************************************\n", 0);

   return (test_count.fail_cnt > 0) ? VAL_STATUS_TEST_FAILED : VAL_STATUS_SUCCESS;
}

