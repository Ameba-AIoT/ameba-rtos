/*
 * Copyright (c) 2024 Realtek, LLC.
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

#include <cstddef>

#include "include/ameba_media_source_usrcfg.h"

// ----------------------------------------------------------------------
//MediaSourceConfig
#if defined MEDIA_PLAYER
extern void *CreateBufferSource(const char *url);
extern void *CreateFileSource(const char *url);
extern void *CreateHTTPSource(const char *url);

MediaSourceConfig kMediaSourceConfigs[] = {
    { "buffer://", 9, CreateBufferSource },
    { "lfs://", 6, CreateFileSource },
    { "vfs://", 6, CreateFileSource },
    { "fat://", 6, CreateFileSource },
#if defined(MEDIA_SOURCE_HTTP)
    { "http://", 7, CreateHTTPSource },
    { "https://", 8, CreateHTTPSource },
#endif
};

size_t kNumMediaSourceConfigs =
    sizeof(kMediaSourceConfigs) / sizeof(kMediaSourceConfigs[0]);
#endif