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

#ifndef _VAL_STORAGE_H_
#define _VAL_STORAGE_H_

#include "val.h"

#define UID_BASE_VALUE  0
#define BYTES_TO_BITS(byte)             (byte * 8)

typedef enum {
    VAL_ITS_SET                         = 0x1,
    VAL_ITS_GET                         = 0x2,
    VAL_ITS_GET_INFO                    = 0x3,
    VAL_ITS_REMOVE                      = 0x4,
    VAL_PS_SET                          = 0x5,
    VAL_PS_GET                          = 0x6,
    VAL_PS_GET_INFO                     = 0x7,
    VAL_PS_REMOVE                       = 0x8,
    VAL_PS_CREATE                       = 0x9,
    VAL_PS_SET_EXTENDED                 = 0xA,
    VAL_PS_GET_SUPPORT                  = 0xB,
    VAL_API_UNUSED                      = 0xC,
} storage_function_type_t;

typedef enum {
    VAL_ITS_FUNCTION                    = 0x0,
    VAL_PS_FUNCTION                     = 0x1,
} storage_function_code_t;

int32_t val_storage_function(int type, ...);

#endif /* _VAL_STORAGE_H_ */
