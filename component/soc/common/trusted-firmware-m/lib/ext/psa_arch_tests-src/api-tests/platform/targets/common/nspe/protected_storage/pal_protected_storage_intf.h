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

#ifndef _PAL_PROTECTED_STORAGE_INTF_H_
#define _PAL_PROTECTED_STORAGE_INTF_H_

#include "pal_common.h"

enum ps_function_code {
    PAL_PS_SET                          = 0x5,
    PAL_PS_GET                          = 0x6,
    PAL_PS_GET_INFO                     = 0x7,
    PAL_PS_REMOVE                       = 0x8,
    PAL_PS_CREATE                       = 0x9,
    PAL_PS_SET_EXTENDED                 = 0xA,
    PAL_PS_GET_SUPPORT                  = 0xB,
};

int32_t pal_ps_function(int type, va_list valist);
#endif /* _PAL_PROTECTED_STORAGE_INTF_H_ */
