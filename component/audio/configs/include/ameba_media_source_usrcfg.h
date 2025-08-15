/*
 * Copyright (c) 2025 Realtek, LLC.
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

#ifndef AMEBA_FWK_MEDIA_PLAYBACK_CONFIG_MEDIA_CONFIG_MEDIA_SOURCE_CONFIG_H
#define AMEBA_FWK_MEDIA_PLAYBACK_CONFIG_MEDIA_CONFIG_MEDIA_SOURCE_CONFIG_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

// ----------------------------------------------------------------------
//MediaSourceConfig
#ifdef MEDIA_PLAYER
typedef void *(*CreateAudioSourceFunc)(const char *);

typedef struct MediaSourceConfig {
    const char *name;
    int prefix_num;
    CreateAudioSourceFunc audio_source_func;
} MediaSourceConfig;

extern MediaSourceConfig kMediaSourceConfigs[];
extern size_t kNumMediaSourceConfigs;
#endif

#ifdef __cplusplus
}
#endif

#endif  // AMEBA_FWK_MEDIA_PLAYBACK_CONFIG_MEDIA_CONFIG_MEDIA_SOURCE_CONFIG_H
