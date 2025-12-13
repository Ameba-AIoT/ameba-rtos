/*
 * Copyright (c) 2025 Realtek, LLC.
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

#ifndef AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_CONDITION_H
#define AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_CONDITION_H

#include <limits.h>

#ifdef __linux__
#include <pthread.h>
#else
#include "FreeRTOS_POSIX.h"
#include "FreeRTOS_POSIX/pthread.h"
#endif

#include "osal_c/osal_errnos.h"
#include "osal_c/osal_macros.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_cond_t {
    pthread_cond_t handle;
} osal_cond_t;

OSAL_STATIC_INLINE
status_t osal_cond_init(osal_cond_t *cond) {
#ifdef __linux__
    pthread_condattr_t attr;
    pthread_condattr_init(&attr);
    pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);

    pthread_cond_init(&cond->handle, &attr);
    pthread_condattr_destroy(&attr);
#else
    pthread_cond_init(&cond->handle, NULL);
#endif
    return OSAL_OK;
}

OSAL_STATIC_INLINE
void osal_cond_destroy(osal_cond_t *cond) {
    pthread_cond_destroy(&cond->handle);
}

OSAL_STATIC_INLINE
status_t osal_cond_wait(osal_cond_t *cond, osal_mutex_t *mutex) {
    return -pthread_cond_wait(&cond->handle, &mutex->handle);
}

OSAL_STATIC_INLINE
status_t osal_cond_wait_relative(osal_cond_t *cond, osal_mutex_t *mutex, int64_t nanosec) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);

    // On 32-bit devices, tv_sec is 32-bit, but `reltime` is 64-bit.
    int64_t reltime_sec = nanosec/1000000000;

    ts.tv_nsec += (long)(nanosec % 1000000000);
    if (reltime_sec < INT64_MAX && ts.tv_nsec >= 1000000000) {
        ts.tv_nsec -= 1000000000;
        ++reltime_sec;
    }

    int64_t time_sec = ts.tv_sec;
    if (time_sec > INT64_MAX - reltime_sec) {
        time_sec = INT64_MAX;
    } else {
        time_sec += reltime_sec;
    }

    ts.tv_sec = (time_sec > LONG_MAX) ? LONG_MAX : (long)time_sec;

    return -pthread_cond_timedwait(&cond->handle, &mutex->handle, &ts);
}

OSAL_STATIC_INLINE
void osal_cond_signal(osal_cond_t *cond) {
    pthread_cond_signal(&cond->handle);
}

OSAL_STATIC_INLINE
void osal_cond_broadcast(osal_cond_t *cond) {
    pthread_cond_broadcast(&cond->handle);
}

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_CONDITION_H
