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

#ifndef AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTMAP_H
#define AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RTMAP_H

#include "osal_c/osal_types.h"

#include "cutils/rttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RTIntMap RTIntMap;
typedef struct RTIntMapEntry RTIntMapEntry;

typedef struct {
	bool valid;
	void *data;
} RTMapEntryInfo;


RTIntMap *RTIntMap_Create(void);
RTIntMap *RTIntMap_CreateEx(RTDestroyFunc destructor);
rt_status_t RTIntMap_Destroy(RTIntMap *self);
rt_status_t RTIntMap_Clear(RTIntMap *self);
rt_status_t RTIntMap_Put(RTIntMap *self, int32_t key, void *value, RTMapEntryInfo *previous);
RTIntMapEntry *RTIntMap_Get(RTIntMap *self, int32_t key);
rt_status_t RTIntMap_Remove(RTIntMap *self, int32_t key, RTMapEntryInfo *entry_info);
bool RTIntMap_HasKey(RTIntMap *self, int32_t key);

int32_t RTIntMapEntry_GetKey(RTIntMapEntry *self);
void *RTIntMapEntry_GetValue(RTIntMapEntry *self);
void RTIntMapEntry_SetValue(RTIntMapEntry *self, void *value);

RTIntMapEntry *RTIntMap_GetFirst(RTIntMap *self);
RTIntMapEntry *RTIntMap_GetLast(RTIntMap *self);
RTIntMapEntry *RTIntMapEntry_GetNext(RTIntMapEntry *entry);
RTIntMapEntry *RTIntMapEntry_GetPrev(RTIntMapEntry *entry);


typedef struct RTRBMap RTRBMap;
typedef struct RTRBMapEntry RTRBMapEntry;

RTRBMap *RTRBMap_Create(RTCompareFunc func);
RTRBMap *RTRBMap_CreateEx(RTCompareFunc func, RTDestroyFunc key_destructor, RTDestroyFunc value_destructor);
rt_status_t RTRBMap_Destroy(RTRBMap *self);
rt_status_t RTRBMap_Clear(RTRBMap *self);
rt_status_t RTRBMap_Put(RTRBMap *self, void *key, void *value, RTMapEntryInfo *previous);
RTRBMapEntry *RTRBMap_Get(RTRBMap *self, void *key);
rt_status_t RTRBMap_Remove(RTRBMap *self, void *key, RTMapEntryInfo *entry_info);
bool RTRBMap_HasKey(RTRBMap *self, void *key);

void *RTRBMapEntry_GetKey(RTRBMapEntry *self);
void *RTRBMapEntry_GetValue(RTRBMapEntry *self);
void RTRBMapEntry_SetValue(RTRBMapEntry *self, void *value);

RTRBMapEntry *RTRBMap_GetFirst(RTRBMap *self);
RTRBMapEntry *RTRBMap_GetLast(RTRBMap *self);
RTRBMapEntry *RTRBMapEntry_GetNext(RTRBMapEntry *entry);
RTRBMapEntry *RTRBMapEntry_GetPrev(RTRBMapEntry *entry);


#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_CUTILS_INTERFACES_CUTILS_RT_MAP_H
