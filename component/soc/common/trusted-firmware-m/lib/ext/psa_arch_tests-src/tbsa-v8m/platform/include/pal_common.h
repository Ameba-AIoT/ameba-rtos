/** @file
 * Copyright (c) 2018, Arm Limited or its affiliates. All rights reserved.
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

#ifndef PAL_COMMON_H
#define PAL_COMMON_H

#include "ARMv8MML.h"

/* Macros */
#define NULL                            ((void *)0)

#ifndef ARMv8MML_H
/* typedef's */
typedef signed   char      int8_t;
typedef unsigned char      uint8_t;
typedef signed   short     int16_t;
typedef unsigned short     uint16_t;
typedef signed   long      int32_t;
typedef unsigned long      uint32_t;
typedef unsigned long long uint64_t;
typedef uint8_t            bool_t;
typedef char *             addr_t;
typedef uint32_t           test_id_t;
typedef char               char8_t;
typedef uint32_t           cfg_id_t;
#else
typedef char *             addr_t;
#define bool_t             uint8_t
#endif

typedef enum
{
    PAL_STATUS_SUCCESS = 0x0,
    PAL_STATUS_ERROR   = 0x80
}pal_status_t;

#endif /* PAL_COMMON_H */
