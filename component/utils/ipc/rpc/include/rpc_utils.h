/*
 * Copyright (c) 2022 Realtek, LLC.
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


#ifndef UTILS_IPC_RPC_UTILS_H
#define UTILS_IPC_RPC_UTILS_H

#include <stdint.h>
#include "os_wrapper.h"

#ifdef __cplusplus
extern "C" {
#endif

void RPC_Print(const char *tag, const char *fmt, ...)
__attribute__((__format__(printf, 2, 3)));

#define RPC_LOGD(...)  ((void)RPC_Print(LOG_TAG, __VA_ARGS__))
#define RPC_LOGE(fmt, ...)  printf(LOG_TAG fmt, ##__VA_ARGS__)

#define ENTER() \
	RPC_LOGD("%s Enter.\n", __func__);

#define RPC_SEM_WAIT_FOREVER 0xFFFFFFFF

typedef struct RPC_Mutex {
	rtos_sema_t sema;
} __attribute__((aligned(4))) RPC_Mutex;

typedef struct RPC_Sem {
	rtos_sema_t semaphore;
	int32_t value;  // semaphore count.
} __attribute__((aligned(4))) RPC_Sem;


void RPC_MutexLock(RPC_Mutex *mutex);

void RPC_MutexUnlock(RPC_Mutex *mutex);

void RPC_MutexInit(RPC_Mutex *mutex);

void RPC_MutexDestroy(RPC_Mutex *mutex);

void RPC_SemInit(RPC_Sem *sem, uint32_t value);

int32_t RPC_SemWait(RPC_Sem *sem, uint32_t ms);

void RPC_SemPost(RPC_Sem *sem);

void RPC_SemDestroy(RPC_Sem *sem);

void RPC_SetLog(int enable);

#ifdef __cplusplus
}
#endif

#endif // UTILS_IPC_RPC_UTILS_H