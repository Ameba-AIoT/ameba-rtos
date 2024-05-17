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

#include "val_framework.h"
#include "val_dispatcher.h"
#include "val_interfaces.h"
#include "val_peripherals.h"
#include "val_target.h"

extern val_api_t val_api;
extern psa_api_t psa_api;

/* gloabls */
addr_t          g_test_info_addr;

/**
    @brief        - This function prints PSA_{SUITE}_API_VERSION_MAJOR
                    PSA_{SUITE}_API_VERSION_MINOR details.
    @param        - None
    @return       - None
**/
__attribute__((unused)) static void val_print_api_version(void)
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
    fn_ptr(&val_api, &psa_api);
    return;
}

/*
    @brief    - Reads the pre-defined component name against given test_id
    @param    - test_id  : Current Test ID
    @return   - Component name
*/
char * val_get_comp_name(test_id_t test_id)
{
    switch (VAL_GET_COMP_NUM(test_id))
    {
        case VAL_FF_BASE:
            return "IPC Suite";
        case VAL_CRYPTO_BASE:
            return "Crypto Suite";
        case VAL_STORAGE_BASE:
            return "Storage Suite";
        case VAL_INITIAL_ATTESTATION_BASE:
            return "Attestation Suite";
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
    boot_t               boot;
    test_count_t         test_count;
    uint32_t             test_result;

    do
    {
        status = val_get_boot_flag(&boot.state);
        if (VAL_ERROR(status))
        {
            return status;
        }

        /* Did last run test hang and system re-booted due to watchdog timeout and
           boot.state was set to BOOT_NOT_EXPECTED ? If yes, set the test status
           to SIM ERROR and go to next test. */
        if (boot.state == BOOT_NOT_EXPECTED)
        {
            val_set_status(RESULT_PENDING(VAL_STATUS_ERROR));
            status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
                return status;
            }
        }
        /* Did last run test hang and system reset due to watchdog timeout but
           boot.state was set to BOOT_EXPECTED_BUT_FAILED ? If yes, set the test status
           to FAIL and go to next test. This condition will hit when test was expecting
           re-boot on perticular scenario but it didn't happen and system re-booted due
           to other reason. */
        else if (boot.state == BOOT_EXPECTED_BUT_FAILED)
        {
            val_set_status(RESULT_FAIL(VAL_STATUS_BOOT_EXPECTED_BUT_FAILED));
            status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                    &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
                return status;
            }
        }
        else
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

            status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_ID_CURRENT),
                                     &test_id, sizeof(test_id_t));
            if (VAL_ERROR(status))
            {
                val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
                return status;
            }

            if (VAL_GET_COMP_NUM(test_id_prev) != VAL_GET_COMP_NUM(test_id))
            {
                val_print(PRINT_ALWAYS, "\nRunning.. ", 0);
                val_print(PRINT_ALWAYS, val_get_comp_name(test_id), 0);
			//	val_print_api_version();
#ifdef TESTS_COVERAGE
                val_print(PRINT_ALWAYS, "\nNOTE : Known failing tests are excluded from this \
build. For PSA functional API certification, all tests must be run.\n", 0);
#endif
                val_print(PRINT_ALWAYS, "\n******************************************\n", 0);
            }

            if (boot.state == BOOT_UNKNOWN)
            {
                /* Set boot.state to BOOT_NOT_EXPECTED to catch unexpected test hang */
                status = val_set_boot_flag(BOOT_NOT_EXPECTED);
                if (VAL_ERROR(status))
                {
                    return status;
                }
            }
            val_execute_test_fn();
        }

        test_result = val_report_status();

        /* Reset boot.state to UNKNOWN before lunching next test */
        status = val_set_boot_flag(BOOT_UNKNOWN);
        if (VAL_ERROR(status))
        {
            return status;
        }

        /* Prepare suite summary data structure */
        status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
            return status;
        }

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

        status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

        test_id_prev = test_id;
        status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_TEST_ID_PREVIOUS),
                                 &test_id, sizeof(test_id_t));
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM write error", 0);
            return status;
        }

   } while(1);

   status = val_nvmem_read(VAL_NVMEM_OFFSET(NV_TEST_CNT), &test_count, sizeof(test_count_t));
   if (VAL_ERROR(status))
   {
       val_print(PRINT_ERROR, "\n\tNVMEM read error", 0);
       return status;
   }

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







