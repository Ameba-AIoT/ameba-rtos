/*
 * Copyright (c) 2021 Realtek, LLC.
 * All rights reserved.
 *
 * Licensed under the Realtek License, Version 1.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License from Realtek
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_TIME_H
#define AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_TIME_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define OSAL_MILLISECONDS_PER_SECOND       (1000LL)           /**< Milliseconds per second. */
#define OSAL_MICROSECONDS_PER_SECOND       (1000000LL)        /**< Microseconds per second. */
#define OSAL_NANOSECONDS_PER_SECOND        (1000000000LL)     /**< Nanoseconds per second. */

#define OSAL_NANOSECONDS_PER_MICROSECOND   (1000LL)           /**< Nanoseconds per microsecond. */
#define OSAL_NANOSECONDS_PER_MILLISECOND   (1000000LL)        /**< Nanoseconds per millisecond. */

typedef int64_t nsecs_t;

typedef enum {
    OSAL_TIME_REALTIME = 0,  /**< system-wide realtime clock */
    OSAL_TIME_MONOTONIC = 1, /**< monotonic time since unspecified starting point */
    OSAL_TIME_PROCESS = 2,   /**< high-resolution per-process clock */
    OSAL_TIME_THREAD = 3,    /**< high-resolution per-thread clock */
    OSAL_TIME_BOOTTIME = 4   /**< same as SYSTEM_TIME_MONOTONIC, but including CPU suspend time */
} osal_clock_t;

typedef struct {
    int64_t sec;   // seconds
    int64_t nsec;  // nano seconds
} osal_timespec;

void osal_sleep(uint32_t sec);
void osal_msleep(uint32_t ms);

int32_t osal_get_time(osal_clock_t clock, osal_timespec *time);
int64_t osal_get_sys_time_ms(osal_clock_t clock);
int64_t osal_get_sys_time_us(osal_clock_t clock);
int64_t osal_get_sys_time_ns(osal_clock_t clock);
int64_t osal_get_debug_time_us(osal_clock_t clock);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_TIME_H
