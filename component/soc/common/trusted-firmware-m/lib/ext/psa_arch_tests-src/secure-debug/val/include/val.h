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

#ifndef _VAL_COMMON_H_
#define _VAL_COMMON_H_

#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <limits.h>
#include <stdarg.h>

typedef uint8_t             bool_t;
typedef uintptr_t           addr_t;
typedef uint32_t            test_id_t;
typedef uint32_t            block_id_t;
typedef char                char8_t;
typedef uint32_t            cfg_id_t;

/* Print verbosity = TEST */
#ifndef VERBOSE
#define VERBOSE 9
#endif

#ifndef __WEAK
#define __WEAK __attribute__((weak))
#endif

#ifndef __UNUSED
#define __UNUSED __attribute__((unused))
#endif

#ifndef TRUE
#define TRUE 0
#endif
#ifndef FALSE
#define FALSE 1
#endif

#define _CONCAT(A, B) A##B
#define CONCAT(A, B) _CONCAT(A, B)

/* test status defines */
#define TEST_START                 0x01
#define TEST_END                   0x02
#define TEST_PASS                  0x04
#define TEST_FAIL                  0x08
#define TEST_SKIP                  0x10
#define TEST_PENDING               0x20

#define TEST_NUM_BIT                 32
#define TEST_STATE_BIT                8
#define TEST_STATUS_BIT               0

#define TEST_NUM_MASK        0xFFFFFFFF
#define TEST_STATE_MASK            0xFF
#define TEST_STATUS_MASK           0xFF

#define RESULT_START(status)    (((TEST_START) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_END(status)      (((TEST_END) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_PASS(status)     (((TEST_PASS) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_FAIL(status)     (((TEST_FAIL) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_SKIP(status)     (((TEST_SKIP) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))
#define RESULT_PENDING(status)  (((TEST_PENDING) << TEST_STATE_BIT) | ((status) << TEST_STATUS_BIT))

#define IS_TEST_FAIL(status)    (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_FAIL)
#define IS_TEST_PASS(status)    (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_PASS)
#define IS_TEST_SKIP(status)    (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_SKIP)
#define IS_TEST_PENDING(status) (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_PENDING)
#define IS_TEST_START(status)   (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_START)
#define IS_TEST_END(status)     (((status >> TEST_STATE_BIT) & TEST_STATE_MASK) == TEST_END)
#define VAL_ERROR(status)       ((status & TEST_STATUS_MASK) ? 1 : 0)

/* Test Defines */
#define TEST_PUBLISH(test_id, entry) \
   const val_test_info_t __attribute__((section(".acs_test_info"))) \
                      CONCAT(acs_test_info, entry) = {test_id, entry}

#define VAL_MAX_TEST_PER_COMP                200
#define VAL_SECURE_DEBUG_BASE                  4

#define VAL_GET_COMP_NUM(test_id)      \
   ((test_id - (test_id % VAL_MAX_TEST_PER_COMP)) / VAL_MAX_TEST_PER_COMP)
#define VAL_GET_TEST_NUM(test_id)      (test_id % VAL_MAX_TEST_PER_COMP)
#define VAL_CREATE_TEST_ID(comp, num)   ((comp*VAL_MAX_TEST_PER_COMP) + num)

#define TEST_FIELD(num1, num2)           (num2 << 8 | num1)

#define TEST_CHECKPOINT_NUM(n)          n
#define TEST(n)                         n
#define BLOCK(n)                        n

#define BLOCK_NUM_POS                   8
#define ACTION_POS                      16
#define GET_TEST_NUM(n)                 (0xff & n)
#define GET_BLOCK_NUM(n)                ((n >> BLOCK_NUM_POS) & 0xff)

#define GET_ACTION_NUM(n)               ((n >> ACTION_POS) & 0xff)
#define TEST_EXECUTE_FUNC               1
#define TEST_RETURN_RESULT              2
#define INVALID_HANDLE                  0x1234DEAD

#define UART_INIT_SIGN  0xff
#define UART_PRINT_SIGN 0xfe

#define TEST_PANIC()                          \
    do {                                         \
    } while (1)

#define TEST_ASSERT_EQUAL(arg1, arg2, checkpoint)                                   \
    do {                                                                            \
        if ((arg1) != arg2)                                                         \
        {                                                                           \
            val->print(PRINT_ERROR, "\tFailed at Checkpoint: %d\n", checkpoint);    \
            val->print(PRINT_ERROR, "\tActual: %d\n", arg1);                        \
            val->print(PRINT_ERROR, "\tExpected: %d\n", arg2);                      \
            return 1;                                                               \
        }                                                                           \
    } while (0)

#define TEST_ASSERT_DUAL(arg1, status1, status2, checkpoint)                        \
    do {                                                                            \
        if ((arg1) != status1 && (arg1) != status2)                                 \
        {                                                                           \
            val->print(PRINT_ERROR, "\tFailed at Checkpoint: %d\n", checkpoint);    \
            val->print(PRINT_ERROR, "\tActual: %d\n", arg1);                        \
            if ((status1) != (status2))                                             \
            {                                                                       \
                val->print(PRINT_ERROR, "\tExpected: %d", status1);                 \
                val->print(PRINT_ERROR, "or %d\n", status2);                        \
            }                                                                       \
            else                                                                    \
            {                                                                       \
                val->print(PRINT_ERROR, "\tExpected: %d\n", status1);               \
            }                                                                       \
            return 1;                                                               \
        }                                                                           \
    } while (0)

#define TEST_ASSERT_NOT_EQUAL(arg1, arg2, checkpoint)                               \
    do {                                                                            \
        if ((arg1) == arg2)                                                         \
        {                                                                           \
            val->print(PRINT_ERROR, "\tFailed at Checkpoint: %d\n", checkpoint);    \
            val->print(PRINT_ERROR, "\tValue: %d\n", arg1);                         \
            return 1;                                                               \
        }                                                                           \
    } while (0)

#define TEST_ASSERT_MEMCMP(buf1, buf2, size, checkpoint)                            \
    do {                                                                            \
        if (memcmp(buf1, buf2, size))                                               \
        {                                                                           \
            val->print(PRINT_ERROR, "\tFailed at Checkpoint: %d : ", checkpoint);   \
            val->print(PRINT_ERROR, "Unequal data in compared buffers\n", 0);       \
            return 1;                                                               \
        }                                                                           \
    } while (0)

#define TEST_ASSERT_RANGE(arg1, range1, range2, checkpoint)                         \
    do {                                                                            \
        if ((arg1) < range1 || (arg1) > range2)                                     \
        {                                                                           \
            val->print(PRINT_ERROR, "\tFailed at Checkpoint: %d\n", checkpoint);    \
            val->print(PRINT_ERROR, "\tActual: %d\n", arg1);                        \
            val->print(PRINT_ERROR, "\tExpected range: %d to ", range1);            \
            val->print(PRINT_ERROR, "%d", range2);                                  \
            return 1;                                                               \
        }                                                                           \
    } while (0)

/* enums */

/* enums to report test sub-state */
typedef enum {
  VAL_STATUS_SUCCESS                     = 0x0,
  VAL_STATUS_INVALID                     = 0x10,
  VAL_STATUS_ERROR                       = 0x11,
  VAL_STATUS_NOT_FOUND                   = 0x12,
  VAL_STATUS_LOAD_ERROR                  = 0x13,
  VAL_STATUS_INSUFFICIENT_SIZE           = 0x14,
  VAL_STATUS_CONNECTION_FAILED           = 0x15,
  VAL_STATUS_CALL_FAILED                 = 0x16,
  VAL_STATUS_READ_FAILED                 = 0x17,
  VAL_STATUS_WRITE_FAILED                = 0x18,
  VAL_STATUS_ISOLATION_LEVEL_NOT_SUPP    = 0x19,
  VAL_STATUS_INIT_FAILED                 = 0x1A,
  VAL_STATUS_SPM_FAILED                  = 0x1B,
  VAL_STATUS_SPM_UNEXPECTED_BEH          = 0x1C,
  VAL_STATUS_FRAMEWORK_VERSION_FAILED    = 0x1D,
  VAL_STATUS_VERSION_API_FAILED          = 0x1E,
  VAL_STATUS_INVALID_HANDLE              = 0x1F,
  VAL_STATUS_INVALID_MSG_TYPE            = 0x20,
  VAL_STATUS_WRONG_IDENTITY              = 0x21,
  VAL_STATUS_MSG_INSIZE_FAILED           = 0x22,
  VAL_STATUS_MSG_OUTSIZE_FAILED          = 0x23,
  VAL_STATUS_SKIP_FAILED                 = 0x24,
  VAL_STATUS_CRYPTO_FAILURE              = 0x25,
  VAL_STATUS_INVALID_SIZE                = 0x26,
  VAL_STATUS_DATA_MISMATCH               = 0x27,
  VAL_STATUS_BOOT_EXPECTED_BUT_FAILED    = 0x28,
  VAL_STATUS_INIT_ALREADY_DONE           = 0x29,
  VAL_STATUS_HEAP_NOT_AVAILABLE          = 0x2A,
  VAL_STATUS_UNSUPPORTED                 = 0x2B,
  VAL_STATUS_DRIVER_FN_FAILED            = 0x2C,
  VAL_STATUS_NO_TESTS                    = 0X2D,
  VAL_STATUS_TEST_FAILED                 = 0x2E,
  VAL_STATUS_ERROR_MAX                   = INT_MAX,
} val_status_t;

/* verbosity enums */
typedef enum {
    PRINT_INFO    = 1,
    PRINT_DEBUG   = 2,
    PRINT_TEST    = 3,
    PRINT_WARN    = 4,
    PRINT_ERROR   = 5,
    PRINT_ALWAYS  = 9
} print_verbosity_t;

/* typedef's */

typedef struct {
    uint32_t pass_cnt:8;
    uint32_t skip_cnt:8;
    uint32_t fail_cnt:8;
    uint32_t sim_error_cnt:8;
} test_count_t;

typedef struct {
    uint16_t test_num;
    uint8_t block_num;
} test_info_t;


/* struture to capture test state */
typedef struct {
    uint16_t reserved;
    uint8_t  state;
    uint8_t  status;
} test_status_buffer_t;

#endif /* VAL_COMMON_H */
