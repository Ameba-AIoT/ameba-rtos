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
	/*
	 * Per-channel diagnostic state (this mutex is shared by the channel's two
	 * receiver threads). Kept here - NOT in globals - so multiple RPC channels
	 * never cross-contaminate each other's hang detection / dump.
	 *   rx_waiting : a receiver thread holds this lock and is idle-blocked in
	 *                ReadBuffer() waiting for the next message (normal, not a hang)
	 *   in_handler : a dispatched handler is currently running
	 *   last_pid/last_proc : program/procedure id of the last dispatched handler
	 */
	volatile int32_t rx_waiting;
	volatile int32_t in_handler;
	volatile unsigned long last_pid;
	volatile unsigned long last_proc;
	/*
	 * Monotonic "a message was dispatched" counter. Lets the stuck detector tell
	 * a real dead-lock (no progress) apart from lock-starvation under heavy load
	 * (the other thread keeps winning the lock, so this counter keeps advancing).
	 */
	volatile unsigned long dispatch_seq;
	/* Number of receiver threads sharing this mutex. On shutdown only the last
	 * thread out destroys/frees it, so the shared mutex is never double-freed. */
	int32_t refcount;
} __attribute__((aligned(4))) RPC_Mutex;

typedef struct RPC_Sem {
	rtos_sema_t semaphore;
	int32_t value;  // semaphore count.
} __attribute__((aligned(4))) RPC_Sem;


void RPC_MutexLock(RPC_Mutex *mutex);

/* Try to lock with a bound (ms). Returns 0 on success, -1 on timeout.
 * Used by the RPC receiver to turn a silent dead-lock into a diagnosable event. */
int32_t RPC_MutexLockTimeout(RPC_Mutex *mutex, uint32_t ms);

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
