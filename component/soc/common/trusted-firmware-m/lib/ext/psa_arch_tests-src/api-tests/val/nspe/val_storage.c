/** @file
 * Copyright (c) 2020, Arm Limited or its affiliates. All rights reserved.
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

#include "val_target.h"
#include "pal_interfaces_ns.h"
#include "val_framework.h"
#include "val_client_defs.h"
#include "val_peripherals.h"
#include "val_storage.h"

/**
    @brief    - This API will call the requested internal trusted storage function
    @param    - type : function code
                ...  : variable number of arguments
    @return   - Error status
**/
int32_t val_storage_function(int type, ...)
{
#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE) || defined(PROTECTED_STORAGE)
    va_list valist;
    int32_t status;

    va_start(valist, type);
    switch (type)
    {
#if defined(STORAGE) || defined(INTERNAL_TRUSTED_STORAGE)
        case VAL_ITS_SET:
        case VAL_ITS_GET:
        case VAL_ITS_GET_INFO:
        case VAL_ITS_REMOVE:
            status = pal_its_function(type, valist);
            break;
#endif
#if defined(STORAGE) || defined(PROTECTED_STORAGE)
        case VAL_PS_SET:
        case VAL_PS_GET:
        case VAL_PS_GET_INFO:
        case VAL_PS_REMOVE:
        case VAL_PS_CREATE:
        case VAL_PS_SET_EXTENDED:
        case VAL_PS_GET_SUPPORT:
            status = pal_ps_function(type, valist);
            break;
#endif
        default:
            val_print(PRINT_ERROR, "\n\nError: Not a valid ITS or PS function code!", 0);
            return VAL_STATUS_ERROR;
    }
    va_end(valist);
    return status;
#else
    (void)type;
    return VAL_STATUS_ERROR;
#endif
}
