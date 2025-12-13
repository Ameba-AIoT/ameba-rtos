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

#ifndef AMEBA_AUDIO_BASE_CUTILS_INCLUDE_CUTILS_CONTAINER_OF_H
#define AMEBA_AUDIO_BASE_CUTILS_INCLUDE_CUTILS_CONTAINER_OF_H

#include <stddef.h>

#define container_of(ptr, type, member) ({               \
    const typeof(((type *)0)->member) *__mptr = (ptr);   \
    (type *)((char *)__mptr - offsetof(type, member));   \
})

#endif // AMEBA_AUDIO_BASE_CUTILS_INCLUDE_CUTILS_CONTAINER_OF_H
