/*
 * Copyright (c) 2025 Realtek Semiconductor Corp.
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

#ifndef AMEBA_AUDIO_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_ATOMIC_H
#define AMEBA_AUDIO_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_ATOMIC_H

#include <stdint.h>

#ifndef OSAL_ATOMIC_INLINE
#define OSAL_ATOMIC_INLINE static inline
#endif

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_inc(volatile uint32_t *addr) {
    return __atomic_fetch_add(addr, 1, __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_dec(volatile uint32_t *addr) {
    return __atomic_fetch_sub(addr, 1, __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_add(uint32_t value, volatile uint32_t *addr) {
    return __atomic_fetch_add(addr, value, __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_and(uint32_t value, volatile uint32_t *addr) {
    return __atomic_fetch_and(addr, value, __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_or(uint32_t value, volatile uint32_t *addr) {
    return __atomic_fetch_or(addr, value, __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_acquire_load(volatile const uint32_t *addr) {
    return __atomic_load_n(addr, __ATOMIC_ACQUIRE);
}

OSAL_ATOMIC_INLINE
uint32_t osal_atomic_release_load(volatile const uint32_t *addr) {
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
    return __atomic_load_n(addr, __ATOMIC_RELAXED);
}

OSAL_ATOMIC_INLINE
void osal_atomic_acquire_store(uint32_t value, volatile uint32_t *addr) {
    __atomic_store_n(addr, value, __ATOMIC_RELAXED);
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
void osal_atomic_release_store(uint32_t value, volatile uint32_t *addr) {
    __atomic_store_n(addr, value, __ATOMIC_RELEASE);
}

OSAL_ATOMIC_INLINE
int osal_atomic_acquire_cas(uint32_t oldvalue, uint32_t newvalue, volatile uint32_t *addr) {
    return !__atomic_compare_exchange_n(addr, &oldvalue, newvalue,
                                        false,
                                        __ATOMIC_ACQUIRE,
                                        __ATOMIC_ACQUIRE);
}

OSAL_ATOMIC_INLINE
int osal_atomic_release_cas(uint32_t oldvalue, uint32_t newvalue, volatile uint32_t *addr) {
    return !__atomic_compare_exchange_n(addr, &oldvalue, newvalue,
                                        false,
                                        __ATOMIC_RELEASE,
                                        __ATOMIC_RELAXED);
}

OSAL_ATOMIC_INLINE
int osal_atomic_compare_add_swap(uint32_t oldvalue, uint32_t newvalue, volatile uint32_t *addr) {
    return __atomic_compare_exchange(addr, &oldvalue, &newvalue,
                                     false,
                                     __ATOMIC_SEQ_CST,
                                     __ATOMIC_SEQ_CST);
}

OSAL_ATOMIC_INLINE
void osal_atomic_compiler_barrier(void) {
    __asm__ __volatile__("" : : : "memory");
}

OSAL_ATOMIC_INLINE
void osal_atomic_memory_barrier(void) {
    __atomic_thread_fence(__ATOMIC_SEQ_CST);
}

#define osal_atomic_write osal_atomic_release_store
#define osal_atomic_cmpxchg osal_atomic_release_cas

#endif // AMEBA_AUDIO_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_ATOMIC_H
