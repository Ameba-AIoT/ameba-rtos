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

#ifndef AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTRINGBUFFER_H
#define AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTRINGBUFFER_H

#include "osal_c/osal_types.h"
#include "cutils/rttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RTRingBuffer RTRingBuffer;

RTRingBuffer *RTRingBuffer_Create(uint32_t size);
rt_status_t   RTRingBuffer_Destroy(RTRingBuffer *rb);

uint32_t RTRingBuffer_Size(RTRingBuffer *rb);
uint32_t RTRingBuffer_Space(RTRingBuffer *rb);
uint32_t RTRingBuffer_Available(RTRingBuffer *rb);

rt_status_t RTRingBuffer_Write(RTRingBuffer *rb, uint8_t *buffer, uint32_t count);
rt_status_t RTRingBuffer_Read(RTRingBuffer *rb, uint8_t *buffer, uint32_t count);

void RTRingBuffer_Reset(RTRingBuffer *rb);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTRINGBUFFER_H
