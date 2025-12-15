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

#ifndef AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MEM_H
#define AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MEM_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OSAL_MEM_COMMON,
    OSAL_MEM_TAG_MAX
} osal_memory_tag_t;

void *osal_malloc_type(osal_memory_tag_t type, size_t size);
void *osal_calloc_type(osal_memory_tag_t type, size_t size);
void *osal_realloc_type(osal_memory_tag_t type, void *p, size_t size);
void  osal_free_type(osal_memory_tag_t type, void *p);

static inline void *osal_malloc(size_t size) {
    return osal_malloc_type(OSAL_MEM_COMMON, size);
}

static inline void *osal_calloc(size_t size) {
    return osal_calloc_type(OSAL_MEM_COMMON, size);
}

static inline void *osal_realloc(void *p, size_t size) {
    return osal_realloc_type(OSAL_MEM_COMMON, p, size);
}

static inline void  osal_free(void *p) {
    osal_free_type(OSAL_MEM_COMMON, p);
}

#ifdef __cplusplus
}
#endif

#endif // AMEBA_BASE_OSAL_OSAL_C_INCLUDE_OSAL_C_OSAL_MEM_H
