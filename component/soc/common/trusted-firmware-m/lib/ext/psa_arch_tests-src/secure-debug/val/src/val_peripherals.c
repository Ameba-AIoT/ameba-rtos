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

#include "val_peripherals.h"
#include "pal_interfaces.h"

/* Global */
uint32_t   is_logger_init_done = 0;

/*
    @brief    - Initialize UART.
                This is client interface API of secure partition UART INIT API.
    @param    - None
    @return   - val_status_t
*/
val_status_t val_logger_init(void)
{
	is_logger_init_done = 1;
	return VAL_STATUS_SUCCESS;
}

/**
    @brief    - Print module. This is client interface API of secure partition
                val_print_sf API for nspe world
    @param    - verbosity: Print verbosity level
              - string   : Input string
              - data     : Value for format specifier
    @return   - val_status_t
**/
val_status_t val_print(print_verbosity_t verbosity, const char *string, int32_t data)
{
    if ((is_logger_init_done == 0) && (verbosity < VERBOSE))
    {
       return VAL_STATUS_SUCCESS;
    }
    return pal_print(string, data);
}



