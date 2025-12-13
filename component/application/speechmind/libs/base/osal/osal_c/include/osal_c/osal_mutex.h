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

#ifndef AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MUTEX_H
#define AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MUTEX_H

#ifdef __linux__
#include <pthread.h>
#else
#include "FreeRTOS_POSIX.h"
#endif

#include "osal_c/osal_errnos.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct osal_mutex_t {
    pthread_mutex_t handle;
} osal_mutex_t;

#define OSAL_DECLARE_MUTEX(mutex) osal_mutex_t mutex

status_t osal_mutex_init(osal_mutex_t *mutex);
status_t osal_mutex_destroy(osal_mutex_t *mutex);
status_t osal_mutex_lock(osal_mutex_t *mutex);
status_t osal_mutex_try_lock(osal_mutex_t *mutex);
status_t osal_mutex_unlock(osal_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MUTEX_H
