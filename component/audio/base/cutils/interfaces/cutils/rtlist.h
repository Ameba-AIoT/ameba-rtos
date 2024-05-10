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

#ifndef AMEBA_BASE_CUTILS_INTERFACES_RTLIST_H
#define AMEBA_BASE_CUTILS_INTERFACES_RTLIST_H

#include "osal_c/osal_types.h"
#include "cutils/rttypes.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct RTList RTList;
typedef struct RTListNode RTListNode;

RTList *RTList_Create(void);
RTList *RTList_CreateEx(RTDestroyFunc destructor);
rt_status_t RTList_Destroy(RTList *self);
rt_status_t RTList_Clear(RTList *self);

RTListNode *RTList_CreateNode(RTList *self);
rt_status_t RTList_DestroyNode(RTList *self, RTListNode *node);

rt_status_t RTList_AppendNode(RTList *self, RTListNode *node);
RTListNode *RTList_AppendData(RTList *self, void *data);

rt_status_t RTList_InsertNode(RTList *self, RTListNode *where, RTListNode *node);
RTListNode *RTList_InsertData(RTList *self, RTListNode *where, void *data);

rt_status_t RTList_RemoveNode(RTList *self, RTListNode *node);
rt_status_t RTList_RemoveData(RTList *self, void *data);
rt_status_t RTList_DetachNode(RTList *self, RTListNode *node);

uint32_t    RTList_Length(RTList *self);
bool        RTList_Empty(RTList *self);
RTListNode *RTList_FirstNode(RTList *self);
RTListNode *RTList_LastNode(RTList *self);
RTListNode *RTList_GetNode(RTList *self, uint32_t index);
RTListNode *RTList_FindData(RTList *self, void *data);
RTListNode *RTList_Find(RTList *self, const void *key, RTEqualsFunc equal_func);

void       *RTListNode_GetData(RTListNode *self);
rt_status_t RTListNode_SetData(RTListNode *self, void *data);
RTListNode *RTListNode_GetNext(RTListNode *self);
RTListNode *RTListNode_GetPrev(RTListNode *self);

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_CUTILS_INTERFACES_RTLIST_H
