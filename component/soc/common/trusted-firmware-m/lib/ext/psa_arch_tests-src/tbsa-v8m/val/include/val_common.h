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

#ifndef _VAL_COMMON_H_
#define _VAL_COMMON_H_

#include <string.h>

#define NULL                            ((void *)0)
#define VAL_PARAM_UNUSED(p)             (void)p
#define BIT16                           (1UL << 16)
#define BIT17                           (1UL << 17)
#define BIT19                           (1UL << 19)
#define BIT22                           (1UL << 22)
#define BIT23                           (1UL << 23)
#define SECURITY_OFFSET                 0x00020000

#ifndef TRUE
#define TRUE  1
#endif

#ifndef FALSE
#define FALSE 0
#endif

/* typedef's */
typedef signed   char       int8_t;
typedef unsigned char       uint8_t;
typedef signed   short      int16_t;
typedef unsigned short      uint16_t;
typedef signed   int        int32_t;
typedef unsigned int        uint32_t;
typedef unsigned long  long uint64_t;
typedef uint8_t             bool_t;
typedef char *              addr_t;
typedef uint32_t            test_id_t;
typedef char                char8_t;
typedef uint32_t            cfg_id_t;

/* enums */
typedef enum {
    TBSA_STATUS_SUCCESS            = 0,
    TBSA_STATUS_ERROR              = 0x80,
    TBSA_STATUS_LOAD_ERROR         = 0x81,
    TBSA_STATUS_ERROR_DEPENDENCY   = 0x82,
    TBSA_STATUS_INSUFFICIENT_SIZE  = 0x83,
    TBSA_STATUS_INVALID_ARGS       = 0x84,
    TBSA_STATUS_INVALID_IRQ_HANDLER= 0x85,
    TBSA_STATUS_INVALID_IRQ_NUMBER = 0x86,
    TBSA_STATUS_UNEXPECTED_EXCP    = 0x87,
    TBSA_STATUS_INCORRECT_ADDRESS  = 0x88,
    TBSA_STATUS_NOT_FOUND          = 0x89,
    TBSA_STATUS_INVALID_AES_MODE   = 0x8A,
    TBSA_STATUS_TIMEOUT            = 0x8B,
    TBSA_STATUS_INCORRECT_VALUE    = 0x8C,
    TBSA_STATUS_SKIP               = 0x8D,
    TBSA_STATUS_INCORRECT_KEY_SIZE = 0x8F,
    TBSA_STATUS_ZERO_COUNT_ERROR   = 0x90,
    TBSA_STATUS_WRITE_ERROR        = 0x91,
    TBSA_STATUS_UART_INIT_ERROR    = 0x92,
    TBSA_STATUS_UNKNOWN            = 0x93,
    TBSA_STATUS_INVALID            = 0xFFFF,
}tbsa_status_t;

typedef enum {
    PRINT_INFO    = 1,
    PRINT_DEBUG   = 2,
    PRINT_TEST    = 3,
    PRINT_WARN    = 4,
    PRINT_ERROR   = 5,
    PRINT_ALWAYS  = 9
}print_verbosity_t;

#endif /* _VAL_COMMON_H_ */
