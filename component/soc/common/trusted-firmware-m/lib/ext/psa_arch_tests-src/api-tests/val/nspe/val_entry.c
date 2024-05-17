/** @file
 * Copyright (c) 2018-2019, Arm Limited or its affiliates. All rights reserved.
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

#include "val_entry.h"
#include "val_framework.h"
#include "val_peripherals.h"
#include "val_dispatcher.h"

#ifdef TGT_DEV_APIS_TFM_AN521
int intermediate_boot = 0;
#define NVMEM_USED_SIZE 16
#endif

/**
    @brief    - PSA C main function, does VAL init and calls test dispatcher
    @param    - None
    @return   - status - error code
**/
int32_t val_entry(void)
{
    test_id_t       test_id;
    int32_t         status;

    status = val_uart_init();
    if (VAL_ERROR(status))
    {
        goto exit;
    }

#ifdef TGT_DEV_APIS_TFM_AN521
    int32_t         init_value[NVMEM_USED_SIZE] = {0};

    if (!intermediate_boot)
    {
        status = val_nvmem_write(VAL_NVMEM_OFFSET(NV_BOOT), init_value, NVMEM_USED_SIZE);
        if (VAL_ERROR(status))
        {
            val_print(PRINT_ERROR, "\n\tNVMEM initialization error", 0);
            goto exit;
        }
    }
#endif

    status = val_get_last_run_test_id(&test_id);
    if (VAL_ERROR(status))
    {
        goto exit;
    }

    /* Compliance header print */
    if (test_id == VAL_INVALID_TEST_ID)
    {
        val_print(PRINT_ALWAYS, "\n***** PSA Architecture Test Suite - Version %d.",
                                                                         PSA_ACS_MAJOR_VER);
        val_print(PRINT_ALWAYS, "%d *****\n", PSA_ACS_MINOR_VER);
    }

    /* Call dispatcher routine*/
    status = val_dispatcher(test_id);

exit:
    val_print(PRINT_ALWAYS, "\nEntering standby.. \n", 0);

    pal_terminate_simulation();

    return status;
}
