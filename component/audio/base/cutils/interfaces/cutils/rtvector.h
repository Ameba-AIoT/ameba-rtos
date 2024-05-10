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

#ifndef AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTVECTOR_H
#define AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTVECTOR_H

#include "osal_c/osal_types.h"
#include "cutils/rttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RTVECTOR_INVALID_INDEX (-1)

typedef struct RTVector RTVector;

RTVector *RTVector_Create();
RTVector *RTVector_CreateEx(RTDestroyFunc destructor);
void RTVector_Destroy(RTVector *self);
void RTVector_Clear(RTVector *self);

rt_status_t RTVector_PushBack(RTVector *self, void *element);
rt_status_t RTVector_PopBack(RTVector *self, bool free);
void  *RTVector_At(RTVector *self, size_t index);
size_t RTVector_Size(RTVector *self);
size_t RTVector_Capacity(RTVector *self);


#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTVECTOR_H
