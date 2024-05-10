/** @file
 * Copyright (c) 2019-2020, Arm Limited or its affiliates. All rights reserved.
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


#include "pal_protected_storage_intf.h"

/**
    @brief    - This API will call the requested protected storage function
    @param    - type    : function code
                valist  : variable argument list
    @return   - error status
**/
int32_t pal_ps_function(int type, va_list valist)
{
    psa_storage_uid_t          uid;
    uint32_t                   data_size, size, offset;
    const void                 *p_write_data;
    void                       *p_read_data;
    size_t                     *p_data_length;
    psa_storage_create_flags_t ps_create_flags;
    struct psa_storage_info_t  *ps_p_info;

    switch (type)
    {
     case PAL_PS_SET:
         uid = va_arg(valist, psa_storage_uid_t);
         data_size = va_arg(valist, uint32_t);
         p_write_data = va_arg(valist, const void*);
         ps_create_flags = va_arg(valist, psa_storage_create_flags_t);
         return psa_ps_set(uid, data_size, p_write_data, ps_create_flags);
     case PAL_PS_GET:
         uid = va_arg(valist, psa_storage_uid_t);
         offset = va_arg(valist, uint32_t);
         data_size = va_arg(valist, uint32_t);
         p_read_data = va_arg(valist, void*);
         p_data_length = va_arg(valist, size_t*);
         return psa_ps_get(uid, offset, data_size, p_read_data, p_data_length);
     case PAL_PS_GET_INFO:
         uid = va_arg(valist, psa_storage_uid_t);
         ps_p_info = va_arg(valist, struct psa_storage_info_t*);
         return psa_ps_get_info(uid, ps_p_info);
     case PAL_PS_REMOVE:
         uid = va_arg(valist, psa_storage_uid_t);
         return psa_ps_remove(uid);
     case PAL_PS_CREATE:
         uid = va_arg(valist, psa_storage_uid_t);
         size = va_arg(valist, uint32_t);
         ps_create_flags = va_arg(valist, psa_storage_create_flags_t);
         return psa_ps_create(uid, size, ps_create_flags);
     case PAL_PS_SET_EXTENDED:
         uid = va_arg(valist, psa_storage_uid_t);
         offset = va_arg(valist, uint32_t);
         data_size = va_arg(valist, uint32_t);
         p_write_data = va_arg(valist, const void*);
         return psa_ps_set_extended(uid, offset, data_size, p_write_data);
     case PAL_PS_GET_SUPPORT:
         return psa_ps_get_support();
    default:
        return PAL_STATUS_UNSUPPORTED_FUNC;
    }

    return PAL_STATUS_UNSUPPORTED_FUNC;
}
